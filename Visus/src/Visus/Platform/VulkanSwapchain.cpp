#include "VulkanSwapchain.h"

#include <Visus/Platform/VulkanGraphicsContext.h>
#include <Visus/Core/Logger.h>

namespace Motus3D
{
	VulkanSwapchain::VulkanSwapchain()
	{
		m_Device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		m_WindowHandle = (GLFWwindow*)VulkanGraphicsContext::GetVulkanContext()->GetSpecification().windowHandle;
		m_FramesInFlight = VulkanGraphicsContext::GetVulkanContext()->GetSpecification().framesInFlight;
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		
	}

	void VulkanSwapchain::Destroy()
	{
		auto vk_instance = VulkanGraphicsContext::GetVulkanContext()->GetInstance();
		// TODO: I don't think it is great idea to wait device idle in every desctructor.
		// Better solution could be adding to deletion queue, before which I will call vkDeviceWaitIdle() once

		vkDeviceWaitIdle(m_Device->GetHandle());
		vkDestroySwapchainKHR(m_Device->GetHandle(), m_Swapchain, nullptr);
		vkDestroySurfaceKHR(vk_instance, m_Surface, nullptr);

		for (auto& image : m_SwapchainImages)
		{
			vkDestroyImageView(m_Device->GetHandle(), image.view, nullptr);
		}

		for (auto& framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(m_Device->GetHandle(), framebuffer, nullptr);
		}

		vkDestroyRenderPass(m_Device->GetHandle(), m_RenderPass, nullptr);

		vkDestroySemaphore(m_Device->GetHandle(), m_Semaphores.presentComplete, nullptr);
		vkDestroySemaphore(m_Device->GetHandle(), m_Semaphores.renderComplete, nullptr);

		for (auto& fence : m_Fences)
		{
			vkDestroyFence(m_Device->GetHandle(), fence, nullptr);
		}

		VISUS_TRACE("Swapchain destroyed");
	}

	void VulkanSwapchain::InitSurface()
	{
		auto vk_instance = VulkanGraphicsContext::GetVulkanContext()->GetInstance();
		auto context_spec = VulkanGraphicsContext::GetVulkanContext()->GetSpecification();

		VK_CHECK_RESULT(glfwCreateWindowSurface(vk_instance, (GLFWwindow*)context_spec.windowHandle, nullptr, &m_Surface));

		VkBool32 is_surface_supported;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_Device->GetPhysicalDevice()->GetHandle(), m_Device->GetPhysicalDevice()->GetQueueFamilyIndices().graphics, m_Surface, &is_surface_supported);

		// TODO: redesign physical device selecting in such way, in which if it doesn't support surface, it will be discarded
		if (is_surface_supported)
		{
			VISUS_TRACE("Surface initialized successfully");
		} else
		{
			VISUS_ERROR("Selected GPU incompatible with surface!");
		}

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device->GetPhysicalDevice()->GetHandle(), m_Surface, &m_SurfaceCapabilities);

		// Selecting surface format and color space
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->GetPhysicalDevice()->GetHandle(), m_Surface, &formatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->GetPhysicalDevice()->GetHandle(), m_Surface, &formatCount, surfaceFormats.data());

		for(int i = 0; i < surfaceFormats.size(); i++)
		{
			if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				m_SurfaceFormat = surfaceFormats[i].format;
				m_SurfaceColorSpace = surfaceFormats[i].colorSpace;
				break;
			}
			if (i == surfaceFormats.size())
			{
				m_SurfaceFormat = surfaceFormats[0].format;
				m_SurfaceColorSpace = surfaceFormats[0].colorSpace;
				VISUS_WARN("Available 32-bit RGBA format with non-linear SRGB color space wasn't found");
			}
		}

		// Enumerating available present modes
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->GetPhysicalDevice()->GetHandle(), m_Surface, &presentModeCount, nullptr);
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->GetPhysicalDevice()->GetHandle(), m_Surface, &presentModeCount, presentModes.data());

		m_PresentModes.first = VK_PRESENT_MODE_FIFO_KHR;

		for(auto& mode : presentModes)
		{
			if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				m_PresentModes.second = mode;
				break;
			}
		}
	}

	void VulkanSwapchain::Create(uint32_t width, uint32_t height, bool vsync)
	{
		m_VSync = vsync;
		m_FramesInFlight = VulkanGraphicsContext::GetVulkanContext()->GetSpecification().framesInFlight;
		m_SwapchainExtent.width = width;
		m_SwapchainExtent.height = height;

		if(m_Swapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(m_Device->GetHandle(), m_Swapchain, nullptr);
			VISUS_TRACE("Destroying incompatible swapchain");
		}

		VkSwapchainCreateInfoKHR swapchain_create_info = {};
		swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_create_info.surface = m_Surface;
		swapchain_create_info.minImageCount = 3;
		swapchain_create_info.clipped = VK_TRUE;
		swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain_create_info.imageFormat = m_SurfaceFormat;
		swapchain_create_info.imageColorSpace = m_SurfaceColorSpace;
		swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_create_info.imageArrayLayers = 1;
		swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if(vsync)
		{
			swapchain_create_info.presentMode = m_PresentModes.first;
		} else
		{
			if (m_PresentModes.second.has_value()) {
				swapchain_create_info.presentMode = m_PresentModes.second.value();
			}
			else
			{
				swapchain_create_info.presentMode = m_PresentModes.first;
				VISUS_WARN("Requested to create swapchain with V-Sync, but current device doesn't support V-Sync!");
			}
		}

		swapchain_create_info.preTransform = m_SurfaceCapabilities.currentTransform;
		swapchain_create_info.imageExtent = { width, height };
		
		VK_CHECK_RESULT(vkCreateSwapchainKHR(m_Device->GetHandle(), &swapchain_create_info, nullptr, &m_Swapchain));

		if(swapchain_create_info.presentMode == VK_PRESENT_MODE_FIFO_KHR)
		{
			VISUS_TRACE("Created swapchain ({0}, {1}, V-Sync On)", width, height);
		} else if (swapchain_create_info.presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			VISUS_TRACE("Created swapchain ({0}, {1}, V-Sync Off)", width, height);
		}

		uint32_t swapchain_image_count;
		vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Swapchain, &swapchain_image_count, nullptr);
		std::vector<VkImage> swapchain_images(swapchain_image_count);
		m_SwapchainImages.resize(swapchain_image_count);
		vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Swapchain, &swapchain_image_count, swapchain_images.data());

		if (m_RenderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(m_Device->GetHandle(), m_RenderPass, nullptr);
		}

		m_Framebuffers.resize(swapchain_image_count);

		VkAttachmentDescription attachment_description = {};
		attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment_description.format = m_SurfaceFormat;

		VkAttachmentReference attachment_reference = {};
		attachment_reference.attachment = 0;
		attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass_description = {};
		subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_description.colorAttachmentCount = 1;
		subpass_description.pColorAttachments = &attachment_reference;

		VkRenderPassCreateInfo render_pass_create_info = {};
		render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_create_info.attachmentCount = 1;
		render_pass_create_info.pAttachments = &attachment_description;
		render_pass_create_info.subpassCount = 1;
		render_pass_create_info.pSubpasses = &subpass_description;

		VK_CHECK_RESULT(vkCreateRenderPass(m_Device->GetHandle(), &render_pass_create_info, nullptr, &m_RenderPass));

		for (auto& image : m_SwapchainImages)
		{
			vkDestroyImageView(m_Device->GetHandle(), image.view, nullptr);
		}

		for(auto& framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(m_Device->GetHandle(), framebuffer, nullptr);
		}

		for (int i = 0; i < m_SwapchainImages.size(); i++)
		{
			m_SwapchainImages[i].image = swapchain_images[i];

			VkImageViewCreateInfo image_view_create_info = {};
			image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			image_view_create_info.pNext = nullptr;
			image_view_create_info.flags = 0;
			image_view_create_info.image = m_SwapchainImages[i].image;
			image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			image_view_create_info.format = m_SurfaceFormat;
			image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_view_create_info.subresourceRange.baseMipLevel = 0;
			image_view_create_info.subresourceRange.levelCount = 1;
			image_view_create_info.subresourceRange.baseArrayLayer = 0;
			image_view_create_info.subresourceRange.layerCount = 1;

			VK_CHECK_RESULT(vkCreateImageView(m_Device->GetHandle(), &image_view_create_info, nullptr, &m_SwapchainImages[i].view));

			// TODO: framebuffer creation
			VkFramebufferCreateInfo framebuffer_create_info = {};
			framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_create_info.renderPass = m_RenderPass;
			framebuffer_create_info.pAttachments = &m_SwapchainImages[i].view;
			framebuffer_create_info.attachmentCount = 1;
			framebuffer_create_info.width = width;
			framebuffer_create_info.height = height;
			framebuffer_create_info.layers = 1;

			VK_CHECK_RESULT(vkCreateFramebuffer(m_Device->GetHandle(), &framebuffer_create_info, nullptr, &m_Framebuffers[i]));

		}

		// Creating sync objects
		// Semaphores
		VkSemaphoreCreateInfo semaphore_create_info = {};
		semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VK_CHECK_RESULT(vkCreateSemaphore(m_Device->GetHandle(), &semaphore_create_info, nullptr, &m_Semaphores.presentComplete));
		VK_CHECK_RESULT(vkCreateSemaphore(m_Device->GetHandle(), &semaphore_create_info, nullptr, &m_Semaphores.renderComplete));

		// Fences
		VkFenceCreateInfo fence_create_info = {};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for(int i = 0; i < m_FramesInFlight; i++)
		{
			VkFence fence;
			VK_CHECK_RESULT(vkCreateFence(m_Device->GetHandle(), &fence_create_info, nullptr, &fence));
			m_Fences.push_back(fence);
		}

	}

	void VulkanSwapchain::BeginFrame()
	{
		vkWaitForFences(m_Device->GetHandle(), 1, &m_Fences[m_CurrentFrameIndex], VK_TRUE, UINT64_MAX);
		vkResetFences(m_Device->GetHandle(), 1, &m_Fences[m_CurrentFrameIndex]);
		TryAcquireNextImage();
	}

	void VulkanSwapchain::EndFrame()
	{
		VkPresentInfoKHR presentinfo = {};
		presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentinfo.pImageIndices = &m_CurrentSwapchainImageIndex;
		presentinfo.swapchainCount = 1;
		presentinfo.pSwapchains = &m_Swapchain;
		presentinfo.waitSemaphoreCount = 1;
		presentinfo.pWaitSemaphores = &m_Semaphores.renderComplete;
		presentinfo.pResults = nullptr;

		VkResult result = vkQueuePresentKHR(m_Device->GetGraphicsTransferQueue(), &presentinfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			int width, height;
			glfwGetFramebufferSize(m_WindowHandle, &width, &height);
			vkQueueWaitIdle(m_Device->GetGraphicsTransferQueue());
			Create(
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height),
				m_VSync
			);
		}

		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_FramesInFlight;
	}

	void VulkanSwapchain::TryAcquireNextImage()
	{
		VkResult result = vkAcquireNextImageKHR(m_Device->GetHandle(), m_Swapchain, UINT64_MAX, m_Semaphores.presentComplete, VK_NULL_HANDLE, &m_CurrentSwapchainImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			int width, height;
			glfwGetFramebufferSize(m_WindowHandle, &width, &height);
			vkQueueWaitIdle(m_Device->GetGraphicsTransferQueue());
			Create(
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height),
				m_VSync
			);
		}
	}
}

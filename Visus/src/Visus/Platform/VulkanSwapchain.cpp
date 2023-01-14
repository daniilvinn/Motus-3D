#include "VulkanSwapchain.h"

#include <Visus/Platform/VulkanGraphicsContext.h>
#include <Visus/Platform/VulkanAllocator.h>
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

		vkDestroySemaphore(m_Device->GetHandle(), m_Semaphores.presentComplete, nullptr);
		vkDestroySemaphore(m_Device->GetHandle(), m_Semaphores.renderComplete, nullptr);

		for (auto& fence : m_Fences)
		{
			vkDestroyFence(m_Device->GetHandle(), fence, nullptr);
		}
		
		auto allocator = VulkanAllocator::Get();
		vkDestroyImageView(m_Device->GetHandle(), m_DepthBuffer.view, nullptr);
		allocator->DestroyImage(m_DepthBuffer.image, m_DepthBuffer.allocation);

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
			if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
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
		swapchain_create_info.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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
		m_SwapchainImages.resize(swapchain_image_count);
		vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Swapchain, &swapchain_image_count, m_SwapchainImages.data());

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

		// DEPTH BUFFER SETUP
		// Cleanup old depth buffer
		auto allocator = VulkanAllocator::Get();
		if (m_DepthBuffer.image != VK_NULL_HANDLE) 
		{
			allocator->DestroyImage(m_DepthBuffer.image, m_DepthBuffer.allocation);
			vkDestroyImageView(m_Device->GetHandle(), m_DepthBuffer.view, nullptr);
		}

		m_DepthBuffer.format = PickDepthFormat();

		VkImageCreateInfo depth_buffer_create_info = {};
		depth_buffer_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depth_buffer_create_info.format = m_DepthBuffer.format;
		depth_buffer_create_info.imageType = VK_IMAGE_TYPE_2D;
		depth_buffer_create_info.extent = { m_SwapchainExtent.width, m_SwapchainExtent.height, 1 };
		depth_buffer_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		depth_buffer_create_info.mipLevels = 1;
		depth_buffer_create_info.arrayLayers = 1;
		depth_buffer_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		depth_buffer_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		depth_buffer_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		
		m_DepthBuffer.allocation = allocator->AllocateImage(&depth_buffer_create_info, 0, &m_DepthBuffer.image);
		
		VkImageViewCreateInfo depth_buffer_view_create_info = {};
		depth_buffer_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depth_buffer_view_create_info.image = m_DepthBuffer.image;
		depth_buffer_view_create_info.format = depth_buffer_create_info.format;
		depth_buffer_view_create_info.components = {
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY
		};
		depth_buffer_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		depth_buffer_view_create_info.subresourceRange.levelCount = 1;
		depth_buffer_view_create_info.subresourceRange.baseMipLevel = 0;
		depth_buffer_view_create_info.subresourceRange.layerCount = 1;
		depth_buffer_view_create_info.subresourceRange.baseArrayLayer = 0;
		depth_buffer_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		
		VK_CHECK_RESULT(vkCreateImageView(m_Device->GetHandle(), &depth_buffer_view_create_info, nullptr, &m_DepthBuffer.view));
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
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
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

	VkFormat VulkanSwapchain::PickDepthFormat()
	{
		// First one is VK_FORMAT_D32_SFLOAT, since it is most preferable format for now.
		std::array<VkFormat, 3> formats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT };

		VkFormatProperties properties;

		for (auto& format : formats) {
			vkGetPhysicalDeviceFormatProperties(
				m_Device->GetPhysicalDevice()->GetHandle(),
				format,
				&properties
			);

			if (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
				return format;
		}
	}

}

#include "VulkanSwapchain.h"

#include <Visus/Platform/VulkanGraphicsContext.h>
#include <Visus/Core/Logger.h>

namespace Visus
{
	VulkanSwapchain::VulkanSwapchain()
	{
		m_Device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		auto vk_instance = VulkanGraphicsContext::GetVulkanContext()->GetInstance();
		// TODO: I don't think it is great idea to wait device idle in every desctructor.
		// Better solution could be adding to deletion queue, before which I will call vkDeviceWaitIdle() once
		vkDeviceWaitIdle(m_Device->GetHandle());
		vkDestroySwapchainKHR(m_Device->GetHandle(), m_Swapchain, nullptr);
		vkDestroySurfaceKHR(vk_instance, m_Surface, nullptr);
	}

	void VulkanSwapchain::InitSurface()
	{
		auto vk_instance = VulkanGraphicsContext::GetVulkanContext()->GetInstance();
		auto context_spec = VulkanGraphicsContext::GetVulkanContext()->GetSpecification();

		VK_CHECK_RESULT(glfwCreateWindowSurface(vk_instance, context_spec.windowHandle, nullptr, &m_Surface));

		//VkBool32 is_surface_supported;
		//vkGetPhysicalDeviceSurfaceSupportKHR(m_Device->GetPhysicalDevice()->GetHandle(), m_Device->GetPhysicalDevice()->GetQueueFamilyIndices().graphics, m_Surface, &is_surface_supported);

		// TODO: redesign physical device selecting in such way, in which if it doesn't support surface, it will be discarded
		//if (is_surface_supported)
		//{
		//	VISUS_ERROR("Selected GPU incompatible with surface!");
		//} else
		//{
		//	VISUS_TRACE("Surface initialized successfully");
		//}
	}
}
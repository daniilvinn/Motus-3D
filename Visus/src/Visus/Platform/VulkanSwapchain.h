#pragma once

#include <Visus/Core/Macros.h>
#include <Visus/Core/Swapchain.h>

#include <Visus/Platform/VulkanDevice.h>

#include <vulkan/vulkan.h>

namespace Visus {
	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain();
		~VulkanSwapchain();

		void InitSurface();
		VkSwapchainKHR GetHandle() { return m_Swapchain; }
		VkSurfaceKHR GetSurfaceHandle() { return m_Surface; }

	private:
		VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

		Ref<VulkanDevice> m_Device;

	};
}
#pragma once

#include <Visus/Core/Macros.h>
#include <Visus/Core/Swapchain.h>
#include <Visus/Platform/VulkanDevice.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


namespace Visus {
	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain();
		~VulkanSwapchain();

		void InitSurface();
		void Create(uint32_t width, uint32_t height, bool vsync = false);

		VkSwapchainKHR GetHandle() { return m_Swapchain; }
		VkSurfaceKHR GetSurfaceHandle() { return m_Surface; }
		VkRenderPass GetRenderPass() { return m_RenderPass; }

		void BeginFrame() override;
		void EndFrame() override;

	private:
		void TryAcquireNextImage();

	private:
		VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		Ref<VulkanDevice> m_Device;
		GLFWwindow* m_WindowHandle;

		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
		VkFormat m_SurfaceFormat;
		VkColorSpaceKHR m_SurfaceColorSpace;
//		\/ \/ \/  FIFO mode is guaranteed to be available, while MAILBOX may be not available, so it is optional. \/ \/ \/
		std::pair<VkPresentModeKHR, std::optional<VkPresentModeKHR>> m_PresentModes;

		struct SwapchainImage
		{
			VkImage image;
			VkImageView view;
		};

		std::vector<SwapchainImage> m_SwapchainImages;
		std::vector<VkFramebuffer> m_Framebuffers;
		uint32_t m_CurrentSwapchainImageIndex;
		uint32_t m_CurrentFrameIndex = 0;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		// Sync objects
		struct Semaphores
		{
			VkSemaphore renderComplete;
			VkSemaphore presentComplete;
		} m_Semaphores;
		std::vector<VkFence> m_Fences;

		bool m_VSync;
		uint8_t m_FramesInFlight;

	};
}
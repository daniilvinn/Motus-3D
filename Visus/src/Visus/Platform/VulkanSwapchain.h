#pragma once

#include <Visus/Core/Macros.h>
#include <Visus/Core/Swapchain.h>
#include <Visus/Platform/VulkanDevice.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


namespace Motus3D {
	class VulkanSwapchain : public Swapchain
	{

	public:
		VulkanSwapchain();
		~VulkanSwapchain();

		void Destroy() override;

		void InitSurface();
		void Create(uint32_t width, uint32_t height, bool vsync = false);

		VkSwapchainKHR GetHandle() { return m_Swapchain; }
		VkSurfaceKHR GetSurfaceHandle() { return m_Surface; }

		struct SwapchainImage
		{
			VkImage image;
			VkImageView view;
		};
		SwapchainImage GetCurrentImageView() { return m_SwapchainImages[m_CurrentSwapchainImageIndex]; }

		std::pair<uint32_t, uint32_t> GetExtent() const override { return {m_SwapchainExtent.width, m_SwapchainExtent.height}; }
		uint8_t GetCurrentFrameIndex() override { return m_CurrentFrameIndex; };

		VkSemaphore GetRenderCompleteSemaphore() { return m_Semaphores.renderComplete; }
		VkSemaphore GetPresentCompleteSemaphore() { return m_Semaphores.presentComplete; }

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
		VkExtent2D m_SwapchainExtent;
		VkFormat m_SurfaceFormat;
		VkColorSpaceKHR m_SurfaceColorSpace;
//		\/ \/ \/  FIFO mode is guaranteed to be available, while MAILBOX may be not available, so it is optional. \/ \/ \/
		std::pair<VkPresentModeKHR, std::optional<VkPresentModeKHR>> m_PresentModes;

		std::vector<SwapchainImage> m_SwapchainImages;
		uint32_t m_CurrentSwapchainImageIndex;
		uint32_t m_CurrentFrameIndex = 0;

		// Sync objects
		struct Semaphores
		{
			VkSemaphore renderComplete;
			VkSemaphore presentComplete;
		} m_Semaphores;
		std::vector<VkFence> m_Fences;

		bool m_VSync;
		uint8_t m_FramesInFlight;

		friend class VulkanRenderer;
	};
}
#pragma once

#include <Visus/Core/GraphicsContext.h>
#include <Visus/Platform/VulkanDevice.h>
#include <Visus/Platform/VulkanSwapchain.h>

#include <vulkan/vulkan.h>
#include <vector>

namespace Motus3D
{
	// Class which represents context for rendering using Vulkan.
	// Call Init() method to initiazlize every renderer component.
	class VulkanGraphicsContext : public GraphicsContext
	{
	public: // Public method
		void Init(const ContextSpecification& spec) override;
		void Shutdown() override;

		static VulkanGraphicsContext* GetVulkanContext() { return s_Instance; }
		ContextSpecification GetSpecification() const { return m_ContextSpecification; }

		VkInstance GetInstance() { return m_VulkanInstance; } // TODO: fix confusing method name
		Ref<VulkanDevice> GetDevice() { return m_Device; }
		Ref<VulkanSwapchain> GetSwapchain() { return m_Swapchain; }

	private: // Private methods
		std::vector<const char*> GetRequiredLayers();
		std::vector<const char*> GetRequiredExtensions();

	private: // Private fields
		static VulkanGraphicsContext* s_Instance;
		ContextSpecification m_ContextSpecification;

		VkInstance m_VulkanInstance;
		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		Ref<VulkanDevice> m_Device;
		Ref<VulkanSwapchain> m_Swapchain;

	};
}
#pragma once

#include <Visus/Core/GraphicsContext.h>
#include <Visus/Platform/VulkanDevice.h>

#include <vulkan/vulkan.h>
#include <vector>

namespace Visus
{
	// Class which represents context for rendering using Vulkan.
	// Call Init() method to initiazlize every renderer component.
	class VulkanGraphicsContext : public GraphicsContext
	{
	public: // Public method
		void Init(const ContextSpecification& spec) override;
		void Shutdown() override;

		static Ref<VulkanGraphicsContext> GetVulkanContext() { return CreateRef<VulkanGraphicsContext>(*s_Instance); };
		VkInstance GetInstance() { return m_VulkanInstance; }
		Ref<VulkanDevice> GetDevice() { return m_Device; }

	private: // Private methods
		static VulkanGraphicsContext* s_Instance;

		std::vector<const char*> GetRequiredLayers();
		std::vector<const char*> GetRequiredExtensions();

	private: // Private fields
		ContextSpecification m_ContextSpecification;
		VkInstance m_VulkanInstance;

		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		Ref<VulkanDevice> m_Device;

	};
}
#pragma once

#include <Visus/Core/GraphicsContext.h>
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

		VkInstance GetInstanceHandle() { return m_Instance; };

	private: // Private methods
		std::vector<const char*> GetRequiredLayers();
		std::vector<const char*> GetRequiredExtensions();

	private: // Private fields
		ContextSpecification m_ContextSpecification;
		VkInstance m_Instance;
		

	};
}
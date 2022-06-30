#include "VulkanGraphicsContext.h"

#include <Visus/Core/Logger.h>
#include <Visus/Platform/VulkanLogger.h>

#include <vulkan/vulkan.h>

namespace Visus
{
	VulkanGraphicsContext* VulkanGraphicsContext::s_Instance = nullptr;

	void VulkanGraphicsContext::Init(const ContextSpecification& spec)
	{
		s_Instance = this;
		m_ContextSpecification = spec;

		glfwInit();

		if(VISUS_INTERNAL_ENABLE_VALIDATION_LAYERS)
		{
			Logger::Init();
		}

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Motus-3D Core";
		appInfo.pEngineName = "Motus-3D Game Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
		appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		std::vector<const char*> enabledLayers = GetRequiredLayers();
		std::vector<const char*> enabledExtensions = GetRequiredExtensions();

		VkInstanceCreateInfo instance_create_info = {};
		instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_create_info.ppEnabledLayerNames = enabledLayers.data();
		instance_create_info.enabledLayerCount = enabledLayers.size();
		instance_create_info.ppEnabledExtensionNames = enabledExtensions.data();
		instance_create_info.enabledExtensionCount = enabledExtensions.size();
		instance_create_info.pApplicationInfo = &appInfo;
		if (VISUS_INTERNAL_ENABLE_VALIDATION_LAYERS) {
			instance_create_info.pNext = VulkanLogger::GetCreateInfo();
		}

		VkResult result = vkCreateInstance(&instance_create_info, nullptr, &m_VulkanInstance);

		if (result == VK_SUCCESS) {
			VISUS_INFO("Graphics context initialized successfully");
		} else
		{
			VISUS_ERROR("Graphics context initialization failed");
		}

		m_PhysicalDevice = VulkanPhysicalDevice::SelectDevice();

		VkPhysicalDeviceFeatures enabledfeatures;
		memset(&enabledfeatures, 0, sizeof(enabledfeatures));
		enabledfeatures.samplerAnisotropy = true;
		enabledfeatures.pipelineStatisticsQuery = true;

		m_Device = CreateRef<VulkanDevice>(m_PhysicalDevice, enabledfeatures);

		m_Swapchain = CreateRef<VulkanSwapchain>();
		m_Swapchain->InitSurface();
	}

	void VulkanGraphicsContext::Shutdown()
	{
		
	}

	std::vector<const char*> VulkanGraphicsContext::GetRequiredLayers()
	{
		std::vector<const char*> layers;
		if (VISUS_INTERNAL_ENABLE_VALIDATION_LAYERS)
		{
			layers.push_back("VK_LAYER_KHRONOS_validation");
		}

		return layers;
	}

	std::vector<const char*> VulkanGraphicsContext::GetRequiredExtensions()
	{
		uint32_t count;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + count);

		if (VISUS_INTERNAL_ENABLE_VALIDATION_LAYERS) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
		
	}
}

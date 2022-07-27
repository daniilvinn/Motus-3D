#include "VulkanGraphicsContext.h"

#include <Visus/Core/Logger.h>
#include <Visus/Platform/VulkanLogger.h>

#include <vulkan/vulkan.h>

#include <Visus/Platform/VulkanAllocator.h>

#include <Visus/Core/Pipeline.h>

namespace Motus3D
{
	VulkanGraphicsContext* VulkanGraphicsContext::s_Instance = nullptr;

	void VulkanGraphicsContext::Init(const ContextSpecification& spec)
	{
		if (s_Instance) VISUS_ERROR("Context already initialized!");
		s_Instance = this;
		m_ContextSpecification = spec;

		glfwInit();

		if(VISUS_INTERNAL_ENABLE_VALIDATION)
		{
			Logger::Init();
		}

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Motus3D-3D Core";
		appInfo.pEngineName = "Motus3D-3D Game Engine";
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
		if (VISUS_INTERNAL_ENABLE_VALIDATION) {
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

		// Getting window's framebuffer size and passing it to swapchain creation
		int windowWidth = 0;
		int windowHeight = 0;
		glfwGetFramebufferSize((GLFWwindow*)spec.windowHandle, &windowWidth, &windowHeight);

		m_Swapchain = CreateRef<VulkanSwapchain>();
		m_Swapchain->InitSurface();
		m_Swapchain->Create(windowWidth, windowHeight, true);

		VulkanAllocator::Init();
	}

	void VulkanGraphicsContext::Shutdown()
	{
		vkDeviceWaitIdle(m_Device->GetHandle());
		m_Swapchain->Destroy();
		m_Device->Destroy();
		vkDestroyInstance(m_VulkanInstance, nullptr);

		VISUS_TRACE("Graphics context destroyed");
	}

	std::vector<const char*> VulkanGraphicsContext::GetRequiredLayers()
	{
		std::vector<const char*> layers;
		if (VISUS_INTERNAL_ENABLE_VALIDATION)
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

		if (VISUS_INTERNAL_ENABLE_VALIDATION) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		return extensions;
		
	}
}

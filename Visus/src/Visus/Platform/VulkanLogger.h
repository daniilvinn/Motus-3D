#pragma once

#include <vulkan/vulkan.h>

namespace Visus
{
	
	class VulkanLogger
	{
	public:
		static void Init(VkInstance instance);
		static void Shutdown(VkInstance instance);

		static VkDebugUtilsMessengerCreateInfoEXT* GetCreateInfo() { return &m_CreateInfo; }

	private:
		static VkDebugUtilsMessengerEXT m_Logger;
		static VkDebugUtilsMessengerCreateInfoEXT m_CreateInfo;

	};
}


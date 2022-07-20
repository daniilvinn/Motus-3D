#pragma once

#include <vulkan/vulkan.h>

namespace Motus3D
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


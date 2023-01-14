#pragma once

#include <string>
#include <Visus/Core/Macros.h>
#include <vulkan/vulkan.h>
#include <Visus/Core/Renderer.h>

#include <vector>

namespace Motus3D {

	struct QueueFamilyIndices
	{
		uint32_t graphics = -1;
		uint32_t compute = -1;
		uint32_t transfer = -1;
	};

	// Physical Device
	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();

		static Ref<VulkanPhysicalDevice> SelectDevice();

		VkPhysicalDevice GetHandle() { return m_PhysicalDevice; }
		VkPhysicalDeviceProperties GetProperties() { return m_Properties; }
		QueueFamilyIndices GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

		bool IsExtensionSupported(std::string ext) const;

	private: // Private Methods
		void FindQueueFamilies();

	private: // Private fields
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties m_Properties;
		QueueFamilyIndices m_QueueFamilyIndices;
		const float m_DefaultQueuePriority = 1.0f;

		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;

		friend class VulkanDevice;
	};

	// Logical Device
	class VulkanDevice
	{
	public:
		VulkanDevice(Ref<VulkanPhysicalDevice> physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
		~VulkanDevice();

		void Destroy();

		VkDevice GetHandle() { return m_Device; }
		Ref<VulkanPhysicalDevice> GetPhysicalDevice() { return m_PhysicalDevice; }
		VkQueue GetGraphicsTransferQueue() { return m_GraphicsTransferQueue; };
		VkQueue GetComputeQueue() { return m_ComputeQueue; };

		VkCommandPool GetDefaultCmdPool() { return m_DefaultCmdPool; }
		VkCommandBuffer GetDefaultCmdBuffer() { return m_DefaultCommandBuffer; }
		void ImmediateExecute(VkCommandBuffer cmd_buffer, QueueFamily family);

	private:
		VkDevice m_Device = VK_NULL_HANDLE;

		Ref<VulkanPhysicalDevice> m_PhysicalDevice;

		VkQueue m_GraphicsTransferQueue = VK_NULL_HANDLE;
		VkQueue m_ComputeQueue = VK_NULL_HANDLE;

		// General purpose command buffer. Recommended for use in one-time submission cases.
		// Owned by graphics-transfer queue family.
		VkCommandPool m_DefaultCmdPool;
		VkCommandBuffer m_DefaultCommandBuffer;

	};

}
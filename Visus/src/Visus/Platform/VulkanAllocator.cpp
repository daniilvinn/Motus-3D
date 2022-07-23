#define VMA_IMPLEMENTATION
#include "VulkanAllocator.h"

#include <Visus/Platform/VulkanGraphicsContext.h>

#include <Visus/Core/Logger.h>

namespace Motus3D
{
	VulkanAllocator* VulkanAllocator::s_Instance = nullptr;

	void VulkanAllocator::Init()
	{
		if (s_Instance != nullptr)
			VISUS_ERROR("Trying to initialize VulkanAllocator twice!");
		else
			s_Instance = new VulkanAllocator();
	}

	VulkanAllocator::VulkanAllocator()
	{
		auto context = VulkanGraphicsContext::GetVulkanContext();

		VmaVulkanFunctions vulkan_functions = {};
		vulkan_functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		vulkan_functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocator_create_info = {};
		allocator_create_info.vulkanApiVersion = VK_API_VERSION_1_3;
		allocator_create_info.instance = context->GetInstance();
		allocator_create_info.physicalDevice = context->GetDevice()->GetPhysicalDevice()->GetHandle();
		allocator_create_info.device = context->GetDevice()->GetHandle();
		allocator_create_info.pVulkanFunctions = &vulkan_functions;

		VK_CHECK_RESULT(vmaCreateAllocator(& allocator_create_info, &m_Allocator));
	}

	VmaAllocation VulkanAllocator::AllocateBuffer(VkBufferCreateInfo* create_info, uint32_t flags, VkBuffer* buffer)
	{
		VmaAllocationCreateInfo allocation_create_info = {};
		allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;
		allocation_create_info.flags = flags;

		VmaAllocation allocation;
		VmaAllocationInfo allocation_info;

		VK_CHECK_RESULT(vmaCreateBuffer(m_Allocator, create_info, &allocation_create_info, buffer, &allocation, &allocation_info));

#ifdef VISUS_DEBUG
		VISUS_TRACE("Allocating buffer: total allocated memory = {0} + {1}", m_Statistics.allocatedMemory, allocation_info.size);
		m_Statistics.allocatedMemory += allocation_info.size;
#endif

		return allocation;
	}

	void VulkanAllocator::DestroyBuffer(VmaAllocation allocation, VkBuffer buffer)
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();

		VmaAllocationInfo allocation_info;
		vmaGetAllocationInfo(m_Allocator, allocation, &allocation_info);

		vkDeviceWaitIdle(device->GetHandle());
		vmaDestroyBuffer(m_Allocator, buffer, allocation);

#ifdef VISUS_DEBUG
		VISUS_TRACE("Destroying buffer: total freed memory = {0} + {1}", m_Statistics.freedMemory, allocation_info.size);
		m_Statistics.freedMemory += allocation_info.size;
#endif

	}

	void* VulkanAllocator::MapMemory(VmaAllocation allocation)
	{
		void* memory;
		vmaMapMemory(m_Allocator, allocation, &memory);
		return memory;
	}

	void VulkanAllocator::UnmapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(m_Allocator, allocation);
	}
}

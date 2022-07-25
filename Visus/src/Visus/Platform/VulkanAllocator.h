#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Motus3D {
	class VulkanAllocator
	{
	public:
		static void Init();
		static VulkanAllocator* Get() { return s_Instance; }

		VmaAllocation AllocateBuffer(VkBufferCreateInfo* create_info, uint32_t flags, VkBuffer* buffer);
		void DestroyBuffer(VmaAllocation allocation, VkBuffer buffer);

		void* MapMemory(VmaAllocation allocation);
		void UnmapMemory(VmaAllocation allocation);

	private:
		VulkanAllocator();

		static VulkanAllocator* s_Instance;
		struct MemoryStatistics
		{
			uint64_t allocatedMemory;
			uint64_t freedMemory;
		} m_Statistics;

		VmaAllocator m_Allocator;

	};
}
#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Motus3D {
	struct MemoryStatistics
	{
		uint64_t allocatedMemory;
		uint64_t freedMemory;
	};

	class VulkanAllocator
	{
	public:
		static void Init();
		static void Shutdown();

		static VulkanAllocator* Get() { return s_Instance; }
		VmaAllocator GetHandle() { return m_Allocator; }
		MemoryStatistics GetStatistics() const { return m_Statistics; }

		VmaAllocation AllocateBuffer(VkBufferCreateInfo* create_info, uint32_t flags, VkBuffer* buffer);
		void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);

		VmaAllocation AllocateImage(VkImageCreateInfo* create_info, uint32_t flags, VkImage* image);
		void DestroyImage(VkImage image, VmaAllocation allocation);

		void* MapMemory(VmaAllocation allocation);
		void UnmapMemory(VmaAllocation allocation);

	private:
		VulkanAllocator();
		~VulkanAllocator();

		VmaAllocator m_Allocator;

		static VulkanAllocator* s_Instance;
		MemoryStatistics m_Statistics;

	};
}

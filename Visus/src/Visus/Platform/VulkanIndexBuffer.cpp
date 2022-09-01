#include "VulkanIndexBuffer.h"

#include "VulkanAllocator.h"
#include "VulkanGraphicsContext.h"

namespace Motus3D
{
	VulkanIndexBuffer::VulkanIndexBuffer(uint64_t size)
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		VkBufferCreateInfo buffer_create_info = {};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.size = size;
		buffer_create_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		auto allocator = VulkanAllocator::Get();
		m_Allocation = allocator->AllocateBuffer(&buffer_create_info, 0, &m_Buffer);

		m_Data.size = size;
	}

	VulkanIndexBuffer::VulkanIndexBuffer(void* data, uint64_t size, uint64_t offset, IndexType type)
		: VulkanIndexBuffer(size)
	{
		SetData(data, size, offset, type);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		Release();
	}

	void VulkanIndexBuffer::Release()
	{
		auto allocator = VulkanAllocator::Get();
		allocator->DestroyBuffer(m_Buffer, m_Allocation);
	}

	void VulkanIndexBuffer::SetData(void* data, uint64_t size, uint64_t offset, IndexType type)
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();

		switch (type)
		{
		case IndexType::UINT8:
			m_Count = size / 1;
			break;
		case IndexType::UINT16:
			m_Count = size / 2;
			break;
		case IndexType::UINT32:
			m_Count = size / 4;
			break;
		default:
			VISUS_ASSERT(false, "Invalid index type!");
			break;
		}

		VISUS_ASSERT(m_Data.size >= size, "Attempted to upload data which doesn't fit buffer's size, possible buffer overflow!");

		// Creating and filling staging buffer
		VkBuffer staging_buffer;
		VkBufferCreateInfo staging_buffer_create_info = {};
		staging_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		staging_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		staging_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		staging_buffer_create_info.size = size;

		auto allocator = VulkanAllocator::Get();
		VmaAllocation staging_buffer_allocation = allocator->AllocateBuffer(
			&staging_buffer_create_info, 
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			&staging_buffer
		);

		void* staging_buffer_memory = allocator->MapMemory(staging_buffer_allocation);
		memcpy(staging_buffer_memory, data, size + offset);
		allocator->UnmapMemory(staging_buffer_allocation);

		// Creating command pool
		VkCommandPool cmd_pool;
		VkCommandPoolCreateInfo cmd_pool_create_info = {};
		cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmd_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		cmd_pool_create_info.queueFamilyIndex = device->GetPhysicalDevice()->GetQueueFamilyIndices().graphics;
		VK_CHECK_RESULT(vkCreateCommandPool(device->GetHandle(), &cmd_pool_create_info, nullptr, &cmd_pool));

		// Allocating command buffers
		VkCommandBuffer cmd_buffer;
		VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {};
		cmd_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmd_buffer_allocate_info.commandPool = cmd_pool;
		cmd_buffer_allocate_info.commandBufferCount = 1;
		cmd_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		VK_CHECK_RESULT(vkAllocateCommandBuffers(device->GetHandle(), &cmd_buffer_allocate_info, &cmd_buffer));

		// Filling command buffer
		VkCommandBufferBeginInfo cmd_command_buffer_begin_info = {};
		cmd_command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmd_command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(cmd_buffer, &cmd_command_buffer_begin_info);

		VkBufferCopy buffer_copy = {};
		buffer_copy.size = size;
		vkCmdCopyBuffer(cmd_buffer, staging_buffer, m_Buffer, 1, &buffer_copy);

		vkEndCommandBuffer(cmd_buffer);

		// Creating fence to wait for, while Vulkan is copying buffer
		VkFence fence;
		VkFenceCreateInfo fence_create_info = {};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		VK_CHECK_RESULT(vkCreateFence(device->GetHandle(), &fence_create_info, nullptr, &fence));

		// Submitting command buffer to GPU
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &cmd_buffer;

		VK_CHECK_RESULT(vkQueueSubmit(device->GetGraphicsTransferQueue(), 1, &submit_info, fence));
		vkWaitForFences(device->GetHandle(), 1, &fence, VK_TRUE, UINT64_MAX);

		vkDestroyCommandPool(device->GetHandle(), cmd_pool, nullptr);
		allocator->DestroyBuffer(staging_buffer, staging_buffer_allocation);
		vkDestroyFence(device->GetHandle(), fence, nullptr);

		m_Data.data = data;
		m_Data.size = size;
		m_IndexType = VisusToVulkanIndexType(type);

	}
}

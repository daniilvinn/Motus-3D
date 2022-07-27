#include "VulkanVertexBuffer.h"
#include "VulkanAllocator.h"

#include "VulkanGraphicsContext.h"

namespace Motus3D {

	VulkanVertexBuffer::VulkanVertexBuffer(uint64_t size)
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();

		VkBufferCreateInfo buffer_create_info = {};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		buffer_create_info.size = size;

		auto allocator = VulkanAllocator::Get();
		m_Allocation = allocator->AllocateBuffer(&buffer_create_info, 0, &m_Buffer);

		VmaAllocationInfo allocation_info = {};
		vmaGetAllocationInfo(allocator->GetHandle(), m_Allocation, &allocation_info);

		VkDebugMarkerObjectNameInfoEXT buffer_name = {};
		buffer_name.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
		buffer_name.objectType = VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT;
		buffer_name.object = (uint64_t)allocation_info.deviceMemory;
		buffer_name.pObjectName = "Vertex Buffer memory";
		
		PFN_vkDebugMarkerSetObjectNameEXT pfnDebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT)vkGetDeviceProcAddr(device->GetHandle(), "vkDebugMarkerSetObjectNameEXT");
		pfnDebugMarkerSetObjectNameEXT(device->GetHandle(), &buffer_name);

		m_Data.size = size;
	}

	VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint64_t size, uint64_t offset)
		: VulkanVertexBuffer(size)
	{
		SetData(data, size, offset);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		auto allocator = VulkanAllocator::Get();
		allocator->DestroyBuffer(&m_Buffer, &m_Allocation);
	}

	void VulkanVertexBuffer::SetData(void* data, uint64_t size, uint64_t offset)
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();

		VISUS_ASSERT(m_Data.size >= size, "Attempted to upload data which doesn't fit buffer's size, possible buffer overflow!");
		m_Data.data = data;

		// Creating and filling staging buffer
		VkBuffer staging_buffer;
		VkBufferCreateInfo staging_buffer_create_info = {};
		staging_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		staging_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		staging_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		staging_buffer_create_info.size = size;

		auto allocator = VulkanAllocator::Get();
		VmaAllocation staging_buffer_allocation = allocator->AllocateBuffer(&staging_buffer_create_info, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, &staging_buffer);

		VmaAllocationInfo staging_buffer_allocation_info = {};
		vmaGetAllocationInfo(allocator->GetHandle(), staging_buffer_allocation, &staging_buffer_allocation_info);

		// Setting up object debug name
		VkDebugMarkerObjectNameInfoEXT staging_buffer_name = {};
		staging_buffer_name.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
		staging_buffer_name.objectType = VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT;
		staging_buffer_name.object = (uint64_t)staging_buffer_allocation_info.deviceMemory;
		staging_buffer_name.pObjectName = "Vertex staging buffer memory";

		PFN_vkDebugMarkerSetObjectNameEXT pfnDebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT)vkGetDeviceProcAddr(device->GetHandle(), "vkDebugMarkerSetObjectNameEXT");
		pfnDebugMarkerSetObjectNameEXT(device->GetHandle(), &staging_buffer_name);

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
		allocator->DestroyBuffer(&staging_buffer, &staging_buffer_allocation);
		vkDestroyFence(device->GetHandle(), fence, nullptr);
	}
}

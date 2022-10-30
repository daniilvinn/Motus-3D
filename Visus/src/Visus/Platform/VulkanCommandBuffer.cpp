#include "VulkanCommandBuffer.h"

#include <Visus/Platform/VulkanGraphicsContext.h>
#include <Visus/Core/Renderer.h>

namespace Motus3D {

	VkCommandPool VulkanCommandBuffer::m_GraphicsCommandPool = VK_NULL_HANDLE;
	VkCommandPool VulkanCommandBuffer::m_ComputeCommandPool = VK_NULL_HANDLE;
	VkCommandPool VulkanCommandBuffer::m_GraphicsTransientCommandPool = VK_NULL_HANDLE;
	VkCommandPool VulkanCommandBuffer::m_ComputeTransientCommandPool = VK_NULL_HANDLE;

	VulkanCommandBuffer::VulkanCommandBuffer(CommandBufferType type, uint32_t flags)
	{
		Allocate(type, flags);
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		Release();
	}

	void VulkanCommandBuffer::InitPools()
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		auto physical_device = device->GetPhysicalDevice();

		VISUS_ASSERT(m_GraphicsCommandPool == VK_NULL_HANDLE, "Attempting to initialize command pools twice!");
		VISUS_ASSERT(m_GraphicsTransientCommandPool == VK_NULL_HANDLE, "Attempting to initialize command pools twice!");
		VISUS_ASSERT(m_ComputeCommandPool == VK_NULL_HANDLE, "Attempting to initialize command pools twice!");
		VISUS_ASSERT(m_ComputeTransientCommandPool == VK_NULL_HANDLE, "Attempting to initialize command pools twice!");

		VkCommandPoolCreateInfo cmd_pool_create_info = {};
		cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmd_pool_create_info.queueFamilyIndex = physical_device->GetQueueFamilyIndices().graphics;
		cmd_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VK_CHECK_RESULT(vkCreateCommandPool(device->GetHandle(), &cmd_pool_create_info, nullptr, &m_GraphicsCommandPool));

		cmd_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(device->GetHandle(), &cmd_pool_create_info, nullptr, &m_GraphicsTransientCommandPool));

		cmd_pool_create_info.queueFamilyIndex = physical_device->GetQueueFamilyIndices().compute;
		cmd_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(device->GetHandle(), &cmd_pool_create_info, nullptr, &m_ComputeCommandPool));

		cmd_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(device->GetHandle(), &cmd_pool_create_info, nullptr, &m_ComputeTransientCommandPool));
	}

	void VulkanCommandBuffer::ReleasePools()
	{
		VISUS_ASSERT(m_GraphicsCommandPool != VK_NULL_HANDLE, "Attempting to release already released or not created pools!");
		VISUS_ASSERT(m_GraphicsTransientCommandPool != VK_NULL_HANDLE, "Attempting to release already released or not created pools!");
		VISUS_ASSERT(m_ComputeCommandPool != VK_NULL_HANDLE, "Attempting to release already released or not created pools!");
		VISUS_ASSERT(m_ComputeTransientCommandPool != VK_NULL_HANDLE, "Attempting to release already released or not created pools!");

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();

		vkDestroyCommandPool(device->GetHandle(), m_GraphicsCommandPool, nullptr);
		vkDestroyCommandPool(device->GetHandle(), m_GraphicsTransientCommandPool, nullptr);
		vkDestroyCommandPool(device->GetHandle(), m_ComputeCommandPool, nullptr);
		vkDestroyCommandPool(device->GetHandle(), m_ComputeTransientCommandPool, nullptr);
	}

	VkCommandBuffer VulkanCommandBuffer::GetHandle()
	{
		return m_CommandBuffers[Renderer::GetCurrentFrameIndex()];
	}

	void VulkanCommandBuffer::Begin()
	{
		VkCommandBufferBeginInfo cmd_buffer_begin_info = {};
		cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if(m_IsTransient) cmd_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(m_CommandBuffers[Renderer::GetCurrentFrameIndex()], &cmd_buffer_begin_info);
	}

	void VulkanCommandBuffer::End()
	{
		vkEndCommandBuffer(m_CommandBuffers[Renderer::GetCurrentFrameIndex()]);
	}

	void VulkanCommandBuffer::Allocate(CommandBufferType type, uint32_t flags)
	{
		m_IsTransient = flags & static_cast<uint32_t>(CommandBufferCreateFlags::TRANSIENT);
		m_Type = type;

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();

		VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {};
		cmd_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmd_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		
		if (m_Type == CommandBufferType::GRAPHICS_TRANSFER && flags == 0) {
			cmd_buffer_allocate_info.commandPool = m_GraphicsCommandPool;
			cmd_buffer_allocate_info.commandBufferCount = 3;
			m_CommandBuffers.resize(3);
		}
		else if (m_Type == CommandBufferType::GRAPHICS_TRANSFER && flags & (uint32_t)CommandBufferCreateFlags::TRANSIENT) {
			cmd_buffer_allocate_info.commandPool = m_GraphicsTransientCommandPool;
			cmd_buffer_allocate_info.commandBufferCount = 1;
			m_CommandBuffers.resize(1);
		}
		else if (m_Type == CommandBufferType::COMPUTE && flags == 0) {
			cmd_buffer_allocate_info.commandPool = m_ComputeCommandPool;
			cmd_buffer_allocate_info.commandBufferCount = 3;
			m_CommandBuffers.resize(3);
		}
		else if (m_Type == CommandBufferType::COMPUTE && flags & (uint32_t)CommandBufferCreateFlags::TRANSIENT) {
			cmd_buffer_allocate_info.commandPool = m_ComputeTransientCommandPool;
			cmd_buffer_allocate_info.commandBufferCount = 1;
			m_CommandBuffers.resize(1);
		}

		VK_CHECK_RESULT(vkAllocateCommandBuffers(device->GetHandle(), &cmd_buffer_allocate_info, m_CommandBuffers.data()));

	}

	void VulkanCommandBuffer::Reset()
	{
		vkResetCommandBuffer(m_CommandBuffers[Renderer::GetCurrentFrameIndex()], 0);
	}

	void VulkanCommandBuffer::Release()
	{
		// Determine which command pool was used to allocate command buffer.
		VkCommandPool cmd_pool;
		if (m_Type == CommandBufferType::GRAPHICS_TRANSFER) {
			cmd_pool = m_GraphicsCommandPool;
			if (m_IsTransient) {
				cmd_pool = m_GraphicsTransientCommandPool;
			}
		}
		else if(m_Type == CommandBufferType::GRAPHICS_TRANSFER) {
			cmd_pool = m_ComputeCommandPool;
			if (m_IsTransient) {
				cmd_pool = m_ComputeTransientCommandPool;
			}
		}
#ifdef VISUS_DEBUG
		else {
			VISUS_ASSERT(false, "trying to release non-allocated command buffer! Use CommandBuffer::Allocate() first!");
		}
#endif

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		vkFreeCommandBuffers(device->GetHandle(), cmd_pool, m_CommandBuffers.size(), m_CommandBuffers.data());

		m_Type = CommandBufferType::NONE;
		m_IsTransient = false;

	}

}
#include "VulkanUniformBuffer.h"

#include <Visus/Platform/VulkanAllocator.h>
#include <Visus/Platform/VulkanGraphicsContext.h>

#include <Visus/Platform/VulkanRenderer.h>

namespace Motus3D 
{

	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding)
		: m_Binding(binding)
	{
		auto allocator = VulkanAllocator::Get();
		m_Size = VulkanRenderer::AlignBuffer(size);

		VkBufferCreateInfo buffer_create_info = {};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		size_t actual_size = m_Size * Renderer::GetConfiguration().framesInFlight;
		buffer_create_info.size = actual_size;

		m_Allocation = allocator->AllocateBuffer(&buffer_create_info, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, &m_Buffer);

	}

	VulkanUniformBuffer::VulkanUniformBuffer(void* data, uint32_t size, uint32_t binding)
		: VulkanUniformBuffer(size, binding)
	{
		SetData(data, size);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		
		auto allocator = VulkanAllocator::Get();
		allocator->DestroyBuffer(m_Buffer, m_Allocation);
	}

	VkWriteDescriptorSet VulkanUniformBuffer::GetWriteDescriptorStruct()
	{
		VkDescriptorBufferInfo buffer_info = {};
		buffer_info.buffer = m_Buffer;
		buffer_info.range = m_Size;
		buffer_info.offset = m_Size * (size_t)(Renderer::GetCurrentFrameIndex() - 1);

		VkWriteDescriptorSet write_struct = {};
		write_struct.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_struct.dstBinding = m_Binding;
		write_struct.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_struct.pBufferInfo = &buffer_info;

		return write_struct;
	}

	void VulkanUniformBuffer::SetData(void* data, uint32_t size)
	{
		auto allocator = VulkanAllocator::Get();

		uint8_t* buffer_memory = (uint8_t*)allocator->MapMemory(m_Allocation);
		uint32_t offset = m_Size * Renderer::GetCurrentFrameIndex();
		memset(buffer_memory + offset, 0, m_Size);
		memcpy(buffer_memory + offset, data, size);
		allocator->UnmapMemory(m_Allocation);
	}

}
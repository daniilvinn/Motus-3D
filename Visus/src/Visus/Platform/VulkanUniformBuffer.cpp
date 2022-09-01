#include "VulkanUniformBuffer.h"

#include <Visus/Platform/VulkanAllocator.h>

namespace Motus3D 
{

	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding)
		: m_Binding(binding)
	{
		auto allocator = VulkanAllocator::Get();

		VkBufferCreateInfo buffer_create_info = {};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		buffer_create_info.size = size;

		m_Allocation = allocator->AllocateBuffer(&buffer_create_info, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, &m_Buffer);

		m_DescriptorBufferInfo.buffer = m_Buffer;
		m_DescriptorBufferInfo.range = size;
		m_DescriptorBufferInfo.offset = 0;

		m_WriteDescriptorStruct = {};
		m_WriteDescriptorStruct.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		m_WriteDescriptorStruct.dstBinding = m_Binding;
		m_WriteDescriptorStruct.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		m_WriteDescriptorStruct.pBufferInfo = &m_DescriptorBufferInfo;

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

	void VulkanUniformBuffer::SetData(void* data, uint32_t size)
	{
		auto allocator = VulkanAllocator::Get();

		void* buffer_memory = allocator->MapMemory(m_Allocation);
		memcpy(buffer_memory, data, size);
		allocator->UnmapMemory(m_Allocation);
	}

}
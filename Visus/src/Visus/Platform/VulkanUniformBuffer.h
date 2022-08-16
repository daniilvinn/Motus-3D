#pragma once

#include <Visus/Core/UniformBuffer.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Motus3D {
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer() {};
		VulkanUniformBuffer(uint32_t size, uint32_t binding);
		VulkanUniformBuffer(void* data, uint32_t size, uint32_t binding);
		~VulkanUniformBuffer();

		VkBuffer GetHandle() { return m_Buffer; }
		VkDescriptorBufferInfo GetDescriptorBufferInfo() { return m_DescriptorBufferInfo; }

		// Returns semi-filled VkWriteDescriptorSet structure. User should fill:
		// - dstSet
		// - dstArrayElement
		// - descriptorCount
		VkWriteDescriptorSet GetWriteDescriptorStruct() { return m_WriteDescriptorStruct; }

		uint32_t GetBinding() { return m_Binding; }
		

		void SetData(void* data, uint32_t size) override;

	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_Allocation;
		VkDescriptorBufferInfo m_DescriptorBufferInfo;
		VkWriteDescriptorSet m_WriteDescriptorStruct;

		uint32_t m_Binding;

	};
}
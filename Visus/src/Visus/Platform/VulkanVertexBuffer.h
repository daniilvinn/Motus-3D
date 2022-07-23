#pragma once

#include <Visus/Core/VertexBuffer.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Motus3D {

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(uint64_t size);
		VulkanVertexBuffer(void* data, uint64_t size, uint64_t offset);

		~VulkanVertexBuffer();

		VkBuffer GetHandle() { return m_Buffer;  }
		void SetData(void* data, uint64_t size, uint64_t offset) override;

	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;

		struct Data
		{
			void* data;
			uint64_t size;
		} m_Data;

	};

}
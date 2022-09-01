#pragma once

#include <Visus/Core/IndexBuffer.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>


namespace Motus3D {

	constexpr VkIndexType VisusToVulkanIndexType(IndexType type)
	{
		switch (type)
		{
		case IndexType::UINT8:
			return VK_INDEX_TYPE_UINT8_EXT;
		case IndexType::UINT16:
			return VK_INDEX_TYPE_UINT16;
		case IndexType::UINT32:
			return VK_INDEX_TYPE_UINT32;
		}

		VISUS_ASSERT(false, "Invalid index type argument");
	}

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint64_t size);
		VulkanIndexBuffer(void* data, uint64_t size, uint64_t offset, IndexType type);
		~VulkanIndexBuffer();

		VkBuffer* GetHandle() { return &m_Buffer; }
		VkIndexType GetVulkanIndexType() const { return m_IndexType; }
		uint32_t GetCount() const override { return m_Count; }

		void Release() override;
		void SetData(void* data, uint64_t size, uint64_t offset, IndexType type);

	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;
		VkIndexType m_IndexType;

		struct Data
		{
			void* data;
			uint64_t size;
		} m_Data;

		uint32_t m_Count;

	};

}
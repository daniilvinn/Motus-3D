#pragma once

#include <Visus/Core/DescriptorSet.h>
#include <vulkan/vulkan.h>

namespace Motus3D {

	constexpr VkDescriptorType VisusToVulkanDescriptorType(ResourceType type) 
	{
		switch (type)
		{
		case ResourceType::UBO:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		case ResourceType::SSBO:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		case ResourceType::IMAGE:
			// Make sure it is correct descriptor type.
			return VK_DESCRIPTOR_TYPE_SAMPLER;
			break;
		default:
			MT_CORE_ASSERT(false, "Invalid descriptor type");
			break;
		}

	}

	class VulkanDescriptorSet : public DescriptorSet 
	{
	public:
		VulkanDescriptorSet(std::initializer_list<Descriptor> layout);
		~VulkanDescriptorSet();
		static void InitPools();
		static void ReleasePools();

		VkDescriptorSet GetHandle() { return m_DescriptorSet; }

		void UpdateDescriptor(uint8_t binding, uint32_t range, uint32_t offset, Ref<UniformBuffer> ubo, uint32_t arrayElement = 0);

	private:
		static VkDescriptorPool s_GlobalDescriptorPool;

		VkDescriptorSet m_DescriptorSet;
		VkDescriptorSetLayout m_DescriptorSetLayout;

	};


}
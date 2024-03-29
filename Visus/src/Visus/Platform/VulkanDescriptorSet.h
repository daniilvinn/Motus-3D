#pragma once

#include <Visus/Core/DescriptorSet.h>
#include <Visus/Core/Cubemap.h>
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
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
		case ResourceType::STORAGE_IMAGE:
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
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

		void Release() override;

		VkDescriptorSet GetHandle() { return m_DescriptorSet; }

		void UpdateDescriptor(uint8_t binding, uint32_t range, uint32_t offset, Ref<UniformBuffer> ubo, uint32_t arrayElement = 0) override;
		void UpdateDescriptor(uint8_t binding, Ref<Image> image, Ref<Sampler> sampler, uint32_t arrayElement = 0) override;
		void UpdateDescriptor(uint8_t binding, Ref<Image> image, uint32_t arrayElement = 0) override;
		void UpdateDescriptor(uint8_t binding, Ref<Cubemap> cubemap, Ref<Sampler> sampler, uint32_t arrayElement = 0) override;

	private:
		static VkDescriptorPool s_GlobalDescriptorPool;

		VkDescriptorSet m_DescriptorSet;
		VkDescriptorSetLayout m_DescriptorSetLayout;

	};


}
#include "VulkanDescriptorSet.h"

#include <Visus/Platform/VulkanGraphicsContext.h>

namespace Motus3D {

	VkDescriptorPool VulkanDescriptorSet::s_DescriptorPool = VK_NULL_HANDLE;

	VulkanDescriptorSet::VulkanDescriptorSet(std::initializer_list<DescriptorSetElement>)
	{
		
	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
		
	}

	void VulkanDescriptorSet::InitializePool()
	{
		std::vector<VkDescriptorPoolSize> pool_sizes = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 10},
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10},
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 10},
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 10},
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 10},
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 10},
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 10},
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 10},
			{ VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK, 10},
			{ VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, 10},
		};

		MT_CORE_ASSERT(s_DescriptorPool == VK_NULL_HANDLE, "Descriptor pool is already initialized!");
		VkDescriptorPoolCreateInfo pool_create_info = {};
		pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_create_info.poolSizeCount = pool_sizes.size();
		pool_create_info.pPoolSizes = pool_sizes.data();
		pool_create_info.maxSets = 100;

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();

		vkCreateDescriptorPool(device->GetHandle(), &pool_create_info, nullptr, &s_DescriptorPool);
	}

}
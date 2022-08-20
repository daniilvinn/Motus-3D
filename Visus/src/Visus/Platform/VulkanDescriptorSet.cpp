#include "VulkanDescriptorSet.h"

#include <Visus/Platform/VulkanUniformBuffer.h>
#include <Visus/Platform/VulkanGraphicsContext.h>

#include <Visus/Platform/VulkanShader.h>
#include <Visus/Platform/VulkanImage.h>

namespace Motus3D {

	VkDescriptorPool VulkanDescriptorSet::s_GlobalDescriptorPool = VK_NULL_HANDLE;

	VulkanDescriptorSet::VulkanDescriptorSet(std::initializer_list<Descriptor> layout /*bool update_after_bind*/) // TODO
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		for (auto& descriptor : layout) 
		{
			VkDescriptorSetLayoutBinding binding = {};
			binding.binding = descriptor.binding;
			binding.descriptorCount = descriptor.arrayElements;
			binding.stageFlags = VisusToVulkanShaderStage(descriptor.stage);
			binding.descriptorType = VisusToVulkanDescriptorType(descriptor.type);

			bindings.push_back(binding);
		}

		VkDescriptorSetLayoutCreateInfo set_layout_create_info = {};
		set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		set_layout_create_info.bindingCount = bindings.size();
		set_layout_create_info.pBindings = bindings.data();
		//set_layout_create_info.flags = update_after_bind ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT : 0;

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device->GetHandle(), &set_layout_create_info, nullptr, &m_DescriptorSetLayout));

		VkDescriptorSetAllocateInfo set_allocate_info = {};
		set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		set_allocate_info.descriptorSetCount = 1;
		set_allocate_info.descriptorPool = s_GlobalDescriptorPool;
		set_allocate_info.pSetLayouts = &m_DescriptorSetLayout;

		VK_CHECK_RESULT(vkAllocateDescriptorSets(device->GetHandle(), &set_allocate_info, &m_DescriptorSet));

	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		vkFreeDescriptorSets(device->GetHandle(), s_GlobalDescriptorPool, 1, &m_DescriptorSet);
		vkDestroyDescriptorSetLayout(device->GetHandle(), m_DescriptorSetLayout, nullptr);
	}

	void VulkanDescriptorSet::InitPools()
	{
		std::vector<VkDescriptorPoolSize> pool_sizes = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 },
			{ VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK, 1000 }
		 // { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1000 },
		 // { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, 1000 },
		 // { VK_DESCRIPTOR_TYPE_MUTABLE_VALVE, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_create_info = {};
		pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_create_info.poolSizeCount = pool_sizes.size();
		pool_create_info.pPoolSizes = pool_sizes.data();
		pool_create_info.maxSets = 1000;

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		VK_CHECK_RESULT(vkCreateDescriptorPool(device->GetHandle(), &pool_create_info, nullptr, &s_GlobalDescriptorPool));

	}

	void VulkanDescriptorSet::ReleasePools()
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		vkDestroyDescriptorPool(device->GetHandle(), s_GlobalDescriptorPool, nullptr);
	}

	void VulkanDescriptorSet::UpdateDescriptor(uint8_t binding, uint32_t range, uint32_t offset, Ref<UniformBuffer> ubo, uint32_t arrayElement /*= 0*/)
	{
		MT_CORE_ASSERT(!(offset > 0 && range == 0), "Out-of-range descriptor: range can't be set as whole size with non-zero offset!");

		Ref<VulkanUniformBuffer> vulkanUBO = RefAs<VulkanUniformBuffer>(ubo);

		VkDescriptorBufferInfo buffer_info = {};
		buffer_info.buffer = vulkanUBO->GetHandle();
		buffer_info.range = range ? range : VK_WHOLE_SIZE;
		buffer_info.offset = offset;

		VkWriteDescriptorSet write_struct = {};
		write_struct.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_struct.dstSet = m_DescriptorSet;
		write_struct.dstBinding = binding;
		write_struct.pBufferInfo = &buffer_info;
		write_struct.dstArrayElement = arrayElement;
		write_struct.descriptorCount = 1;
		write_struct.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		vkUpdateDescriptorSets(device->GetHandle(), 1, &write_struct, 0, nullptr);
	}

	void VulkanDescriptorSet::UpdateDescriptor(uint8_t binding, Ref<Image> image, Ref<Sampler> sampler, uint32_t arrayElement)
	{
		Ref<VulkanImage> vulkanImage = RefAs<VulkanImage>(image);
		Ref<VulkanSampler> vulkanSampler = RefAs<VulkanSampler>(sampler);

		VkDescriptorImageInfo image_info = {};
		image_info.imageView = vulkanImage->GetImageView();
		image_info.sampler = vulkanSampler->GetHandle();
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet write_struct = {};
		write_struct.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_struct.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_struct.dstSet = m_DescriptorSet;
		write_struct.dstBinding = binding;
		write_struct.pImageInfo = &image_info;
		write_struct.dstArrayElement = arrayElement;
		write_struct.descriptorCount = 1;

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		vkUpdateDescriptorSets(device->GetHandle(), 1, &write_struct, 0, nullptr);
	}
}
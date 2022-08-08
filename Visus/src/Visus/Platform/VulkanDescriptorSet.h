#pragma once

#include <Visus/Core/DescriptorSet.h>
#include <vulkan/vulkan.h>

namespace Motus3D {
	class VISUS_API VulkanDescriptorSet : public DescriptorSet
	{
	public:
		VulkanDescriptorSet(std::initializer_list<DescriptorSetElement>);
		~VulkanDescriptorSet();

		static void InitializePool();

	private:
		static VkDescriptorPool s_DescriptorPool;

		VkDescriptorSet m_DescriptorSet;
	};
}
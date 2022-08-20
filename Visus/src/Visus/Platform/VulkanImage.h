#pragma once

#include <Visus/Core/Image.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Motus3D {
	class VulkanImage : public Image
	{
	public:
		VulkanImage(std::string filepath);
		~VulkanImage();

		VkImage GetHandle() { return m_Image; }
		VkImageView GetImageView() { return m_ImageView; }

	private:
		VkImage m_Image;
		VmaAllocation m_Allocation;

		VkImageView m_ImageView;
	};

	class VulkanSampler : public Sampler 
	{
	public:
		VulkanSampler(SamplerSpecification specification);

		VkSampler GetHandle() { return m_Sampler; }

	private:
		VkSampler m_Sampler;
	};

}
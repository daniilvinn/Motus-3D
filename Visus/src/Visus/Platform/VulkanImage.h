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

		std::string_view GetFilepath() const override { return m_Filepath; };

		VkImage GetHandle() { return m_Image; }
		VkImageView GetImageView() { return m_ImageView; }

	private:
		VkImage m_Image;
		VmaAllocation m_Allocation;
		VkImageView m_ImageView;

		std::string m_Filepath;

	};

	class VulkanSampler : public Sampler 
	{
	public:
		VulkanSampler(SamplerSpecification specification);
		~VulkanSampler();

		void Destroy() override;
		VkSampler GetHandle() { return m_Sampler; }

	private:
		VkSampler m_Sampler;
	};

}
#pragma once

#include <Visus/Core/Image.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Motus3D {

	class VulkanImage : public Image
	{
	public:
		VulkanImage(std::string filepath, ImageFormat format);
		~VulkanImage();

		std::string_view GetFilepath() const override { return m_Filepath; };

		VkImage GetHandle() { return m_Image; }
		VkImageView GetImageView() { return m_ImageView; }

		void Release() override;

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

	constexpr VkFormat VisusToVulkanImageFormat(ImageFormat format) {

		switch (format)
		{
		case ImageFormat::C8:
			return VK_FORMAT_R8G8B8A8_SRGB;
			break;
		case ImageFormat::C16:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
			break;
		case ImageFormat::C32:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		case ImageFormat::D32:
			return VK_FORMAT_D32_SFLOAT;
			break;
		case ImageFormat::D24S8:
			return VK_FORMAT_D24_UNORM_S8_UINT;
			break;
		case ImageFormat::D32S8:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
			break;
		default:
			VISUS_ASSERT(false, "\"NONE\" can not be used as image format!");
			break;
		}

	}

}
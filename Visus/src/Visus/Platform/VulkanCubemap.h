#pragma once

#include <Visus/Core/Macros.h>
#include <Visus/Core/Cubemap.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Motus3D {

	class VulkanCubemap : public Cubemap {
	public:
		VulkanCubemap();
		VulkanCubemap(std::string filepath);
		~VulkanCubemap();

		std::string_view GetFilepath() const override { return m_Filepath; }
		void Load(std::string_view filepath) override;
		void Release() override;


		VkImage GetHandle() { return m_Image; }
		VkImageView GetImageView() { return m_ImageView; }

	private:
		VkImage m_Image;
		VmaAllocation m_Allocation;

		VkImageView m_ImageView;

		std::string_view m_Filepath;

	};

}
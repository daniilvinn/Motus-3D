#include "VulkanCubemap.h"

#include <Visus/Platform/VulkanGraphicsContext.h>
#include <Visus/Platform/VulkanAllocator.h>
#include <Visus/Platform/VulkanRenderer.h>

#include <stb_image.h>

namespace Motus3D {

	VulkanCubemap::VulkanCubemap()
	{

	}

	VulkanCubemap::VulkanCubemap(std::string directory)
		: m_Filepath(directory)
	{
		Load(directory);
	}

	VulkanCubemap::~VulkanCubemap()
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		vkDestroyImageView(device->GetHandle(), m_ImageView, nullptr);

		auto allocator = VulkanAllocator::Get();
		allocator->DestroyImage(m_Image, m_Allocation);
	}

	void VulkanCubemap::Load(std::string_view filepath)
	{
		int imageWidth;
		int imageHeight;
		int ChannelCount;
		stbi_uc* pixel_data[6];

		std::string string_filepath(filepath);

		pixel_data[0] = stbi_load((string_filepath + "/posx.jpg").c_str(), &imageWidth, &imageHeight, &ChannelCount, STBI_rgb_alpha);
		pixel_data[1] = stbi_load((string_filepath + "/negx.jpg").c_str(), &imageWidth, &imageHeight, &ChannelCount, STBI_rgb_alpha);
		pixel_data[2] = stbi_load((string_filepath + "/posy.jpg").c_str(), &imageWidth, &imageHeight, &ChannelCount, STBI_rgb_alpha);
		pixel_data[3] = stbi_load((string_filepath + "/negy.jpg").c_str(), &imageWidth, &imageHeight, &ChannelCount, STBI_rgb_alpha);
		pixel_data[4] = stbi_load((string_filepath + "/posz.jpg").c_str(), &imageWidth, &imageHeight, &ChannelCount, STBI_rgb_alpha);
		pixel_data[5] = stbi_load((string_filepath + "/negz.jpg").c_str(), &imageWidth, &imageHeight, &ChannelCount, STBI_rgb_alpha);

		// Check for successful cubemap load
		bool load_failed = false;
		for (int i = 0; i < 6; i++) {
			if (!pixel_data[i]) {
				VISUS_ASSERT(false, "Failed to cubemap face #{i}", i);
				load_failed = true;
			}
		}

		if (load_failed)
			return;

		VkBuffer staging_buffer;
		VkBufferCreateInfo staging_buffer_create_info = {};
		staging_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		staging_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		staging_buffer_create_info.size = imageWidth * imageHeight * 4 * 6;
		staging_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		auto allocator = VulkanAllocator::Get();
		VmaAllocation staging_buffer_allocation = allocator->AllocateBuffer(&staging_buffer_create_info, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, &staging_buffer);

		uint8_t* staging_buffer_memory = (uint8_t*)allocator->MapMemory(staging_buffer_allocation);
		for (int i = 0; i < 6; i++) {
			memcpy(staging_buffer_memory + (imageWidth * imageHeight * 4 * i), pixel_data[i], imageWidth * imageHeight * 4);
		}
		allocator->UnmapMemory(staging_buffer_allocation);

		VkImageCreateInfo image_create_info = {};
		image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_create_info.imageType = VK_IMAGE_TYPE_2D;
		image_create_info.extent = { static_cast<uint32_t>(imageWidth), static_cast<uint32_t>(imageHeight), 1 };
		image_create_info.mipLevels = 1;
		image_create_info.arrayLayers = 6;
		image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
		image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		image_create_info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

		m_Allocation = allocator->AllocateImage(&image_create_info, 0, &m_Image);

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		auto cmd_buffer = VulkanGraphicsContext::GetVulkanContext()->GetDevice()->GetDefaultCmdBuffer();

		VkCommandBufferBeginInfo cmd_buffer_begin_info = {};
		cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		std::array<VkBufferImageCopy, 6> buffer_image_copies;

		for (int i = 0; i < 6; i++) {
			buffer_image_copies[i].bufferOffset = 0 + (imageWidth * imageHeight * 4 * i);
			buffer_image_copies[i].bufferRowLength = 0;
			buffer_image_copies[i].bufferImageHeight = 0;

			buffer_image_copies[i].imageExtent = { static_cast<uint32_t>(imageWidth), static_cast<uint32_t>(imageHeight), 1 };
			buffer_image_copies[i].imageOffset = { 0, 0, 0 };
			buffer_image_copies[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			buffer_image_copies[i].imageSubresource.mipLevel = 0;
			buffer_image_copies[i].imageSubresource.layerCount = 1;
			buffer_image_copies[i].imageSubresource.baseArrayLayer = i;
		}

		vkBeginCommandBuffer(cmd_buffer, &cmd_buffer_begin_info);
			VulkanRenderer::TransitionImageLayout(cmd_buffer, m_Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, buffer_image_copies.size());
			vkCmdCopyBufferToImage(cmd_buffer, staging_buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, buffer_image_copies.size(), buffer_image_copies.data());
			VulkanRenderer::TransitionImageLayout(cmd_buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, buffer_image_copies.size());
		vkEndCommandBuffer(cmd_buffer);

		VkFence wait_fence;
		VkFenceCreateInfo fence_create_info = {};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		VK_CHECK_RESULT(vkCreateFence(device->GetHandle(), &fence_create_info, nullptr, &wait_fence));

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &cmd_buffer;

		vkQueueSubmit(device->GetGraphicsTransferQueue(), 1, &submit_info, wait_fence);
		vkWaitForFences(device->GetHandle(), 1, &wait_fence, VK_TRUE, UINT64_MAX);

		vkDestroyFence(device->GetHandle(), wait_fence, nullptr);
		allocator->DestroyBuffer(staging_buffer, staging_buffer_allocation);
		vkResetCommandPool(device->GetHandle(), device->GetDefaultCmdPool(), 0); // Remind: do NOT forget to reset default cmd buffer!

		VkImageViewCreateInfo image_view_create_info = {};
		image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_create_info.image = m_Image;
		image_view_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
		image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		image_view_create_info.components = {
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY
		};
		image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_view_create_info.subresourceRange.baseArrayLayer = 0;
		image_view_create_info.subresourceRange.layerCount = 6;
		image_view_create_info.subresourceRange.baseMipLevel = 0;
		image_view_create_info.subresourceRange.levelCount = 1;

		VK_CHECK_RESULT(vkCreateImageView(device->GetHandle(), &image_view_create_info, nullptr, &m_ImageView));
	}

}
#include "VulkanImage.h"

#include <Visus/Platform/VulkanAllocator.h>

#include <Visus/Platform/VulkanRenderer.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Motus3D {

	// ============
	// Vulkan Image

	VulkanImage::VulkanImage(std::string filepath)
		: m_Filepath(filepath.c_str())
	{
		int imageWidth;
		int imageHeight;
		int ChannelCount;
		stbi_uc* pixel_data = stbi_load(filepath.c_str(), &imageWidth, &imageHeight, &ChannelCount, STBI_rgb_alpha);

		VkBuffer staging_buffer;
		VkBufferCreateInfo staging_buffer_create_info = {};
		staging_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		staging_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		staging_buffer_create_info.size = imageWidth * imageHeight * 4;
		staging_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		auto allocator = VulkanAllocator::Get();
		VmaAllocation staging_buffer_allocation = allocator->AllocateBuffer(&staging_buffer_create_info, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, &staging_buffer);

		void* staging_buffer_memory = allocator->MapMemory(staging_buffer_allocation);
		memcpy(staging_buffer_memory, pixel_data, staging_buffer_create_info.size);
		allocator->UnmapMemory(staging_buffer_allocation);

		VkImageCreateInfo image_create_info = {};
		image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_create_info.imageType = VK_IMAGE_TYPE_2D;
		image_create_info.extent = {static_cast<uint32_t>(imageWidth), static_cast<uint32_t>(imageHeight), 1};
		image_create_info.mipLevels = 1;
		image_create_info.arrayLayers = 1;
		image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
		image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		m_Allocation = allocator->AllocateImage(&image_create_info, 0, &m_Image);

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		auto cmd_buffer = VulkanGraphicsContext::GetVulkanContext()->GetDevice()->GetDefaultCmdBuffer();

		VkCommandBufferBeginInfo cmd_buffer_begin_info = {};
		cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkBufferImageCopy buffer_image_copy = {};
		buffer_image_copy.bufferOffset = 0;
		buffer_image_copy.bufferRowLength = 0;
		buffer_image_copy.bufferImageHeight = 0;

		buffer_image_copy.imageExtent = { static_cast<uint32_t>(imageWidth), static_cast<uint32_t>(imageHeight), 1 };
		buffer_image_copy.imageOffset = { 0, 0, 0 };
		buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		buffer_image_copy.imageSubresource.mipLevel = 0;
		buffer_image_copy.imageSubresource.layerCount = 1;
		buffer_image_copy.imageSubresource.baseArrayLayer = 0;

		vkBeginCommandBuffer(cmd_buffer, &cmd_buffer_begin_info);
			VulkanRenderer::TransitionImageLayout(cmd_buffer, m_Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			vkCmdCopyBufferToImage(cmd_buffer, staging_buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);
			VulkanRenderer::TransitionImageLayout(cmd_buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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
		vkResetCommandPool(device->GetHandle(), device->GetDefaultCmdPool(), 0);

		VkImageViewCreateInfo image_view_create_info = {};
		image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_create_info.image = m_Image;
		image_view_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
		image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		image_view_create_info.components = {
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY
		};
		image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_view_create_info.subresourceRange.baseArrayLayer = 0;
		image_view_create_info.subresourceRange.layerCount = 1;
		image_view_create_info.subresourceRange.baseMipLevel = 0;
		image_view_create_info.subresourceRange.levelCount = 1;
		
		STBI_FREE(pixel_data);
		VK_CHECK_RESULT(vkCreateImageView(device->GetHandle(), &image_view_create_info, nullptr, &m_ImageView));

	}

	VulkanImage::~VulkanImage()
	{
		Release();
	}

	void VulkanImage::Release()
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		auto allocator = VulkanAllocator::Get();
		vkDeviceWaitIdle(device->GetHandle());
		vkDestroyImageView(device->GetHandle(), m_ImageView, nullptr);
		allocator->DestroyImage(m_Image, m_Allocation);
	}

	// ==============
	// Vulkan Sampler
	static constexpr VkFilter VisusToVulkanSamplerFilter(SamplerFilter filter) 
	{
		switch (filter)
		{
		case SamplerFilter::NEAREST:
			return VK_FILTER_NEAREST;
			break;
		case SamplerFilter::LINEAR:
			return VK_FILTER_LINEAR;
			break;
		default:
			MT_CORE_ASSERT(false, "Invalid sampler filter!");
			break;
		}
	}

	static constexpr VkSamplerMipmapMode VisusToVulkanSamplerMipmapMode(MipmapMode mode) 
	{
		switch (mode)
		{
		case MipmapMode::NEAREST:
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			break;
		case MipmapMode::LINEAR:
			return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			break;
		default:
			MT_CORE_ASSERT(false, "Invalid sampler mipmap mode!");
			break;
		}
	}

	static constexpr VkSamplerAddressMode VisusToVulkanSamplerAddressMode(SamplerAddressMode mode) 
	{
		switch (mode)
		{
		case SamplerAddressMode::REPEAT:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			break;
		case SamplerAddressMode::MIRRORED_REPEAT:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			break;
		case SamplerAddressMode::CLAMP:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			break;
		case SamplerAddressMode::CLAMP_BORDER:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			break;
		default:
			MT_CORE_ASSERT(false, "Invalid sampler addressing mode!");
			break;
		}
	}

	VulkanSampler::VulkanSampler(SamplerSpecification specification)
	{
		VkSamplerCreateInfo sampler_create_info = {};
		sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_create_info.minFilter = VisusToVulkanSamplerFilter(specification.minFilter);
		sampler_create_info.magFilter = VisusToVulkanSamplerFilter(specification.magFilter);
		sampler_create_info.addressModeU = VisusToVulkanSamplerAddressMode(specification.addressMode);
		sampler_create_info.addressModeV = VisusToVulkanSamplerAddressMode(specification.addressMode);
		sampler_create_info.addressModeW = VisusToVulkanSamplerAddressMode(specification.addressMode);
		sampler_create_info.anisotropyEnable = specification.anisotropy == 1.0f ? VK_FALSE : VK_TRUE;
		sampler_create_info.maxAnisotropy = specification.anisotropy;
		sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_create_info.unnormalizedCoordinates = VK_FALSE;
		sampler_create_info.compareEnable = VK_FALSE;
		sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_create_info.mipmapMode = VisusToVulkanSamplerMipmapMode(specification.mipmapMode);
		sampler_create_info.mipLodBias = 0.0f;
		sampler_create_info.minLod = 0.0f;
		sampler_create_info.maxLod = 0.0f;

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		VK_CHECK_RESULT(vkCreateSampler(device->GetHandle(), &sampler_create_info, nullptr, &m_Sampler));
	}

	VulkanSampler::~VulkanSampler()
	{
		Destroy();
	}

	void VulkanSampler::Destroy()
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		vkDestroySampler(device->GetHandle(), m_Sampler, nullptr);
	}

}
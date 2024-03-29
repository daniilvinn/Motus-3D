#include "VulkanRenderer.h"

#include <Visus/Core/Renderer.h>

#include <Visus/Platform/VulkanVertexBuffer.h>
#include <Visus/Platform/VulkanIndexBuffer.h>
#include <Visus/Platform/VulkanPipeline.h>
#include <Visus/Platform/VulkanUniformBuffer.h>
#include <Visus/Platform/VulkanDescriptorSet.h>
#include <Visus/Platform/VulkanCommandBuffer.h>
#include <Visus/Platform/VulkanImage.h>


#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace Motus3D {

	VulkanRenderer::VulkanRenderer()
	{
		auto renderer_config = Renderer::GetConfiguration();
		m_GraphicsContext = CreateRef<VulkanGraphicsContext>();
		m_GraphicsContext->Init({
			renderer_config.windowHandle,
			renderer_config.framesInFlight
		});

		m_Device = m_GraphicsContext->GetDevice();
		m_Swapchain = m_GraphicsContext->GetSwapchain();

		for (int i = 0; i < renderer_config.framesInFlight; i++)
		{
			VkCommandPoolCreateInfo cmd_pool_create_info = {};
			cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmd_pool_create_info.queueFamilyIndex = m_Device->GetPhysicalDevice()->GetQueueFamilyIndices().graphics;
			cmd_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

			VkCommandPool cmd_pool;
			VK_CHECK_RESULT(vkCreateCommandPool(m_Device->GetHandle(), &cmd_pool_create_info, nullptr, &cmd_pool));

			VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {};
			cmd_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmd_buffer_allocate_info.commandBufferCount = 1;
			cmd_buffer_allocate_info.commandPool = cmd_pool;
			cmd_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

			VkCommandBuffer cmd_buffer;
			vkAllocateCommandBuffers(m_Device->GetHandle(), &cmd_buffer_allocate_info, &cmd_buffer);

			m_CommandBuffers.push_back({ cmd_pool, cmd_buffer });
		}
	}

	VulkanRenderer::~VulkanRenderer()
	{
		vkQueueWaitIdle(m_Device->GetGraphicsTransferQueue());
		for (auto& cmd : m_CommandBuffers)
		{
			vkDestroyCommandPool(m_Device->GetHandle(), cmd.pool, nullptr);
		}

		m_GraphicsContext->Shutdown();
	}

	void VulkanRenderer::OnWindowResize(uint32_t width, uint32_t height, bool vsync)
	{
		m_Swapchain->Create(width, height, vsync);
	}

	void VulkanRenderer::BeginFrame()
	{
		m_Swapchain->BeginFrame();

		m_CurrentCommandBuffer = m_CommandBuffers[m_Swapchain->GetCurrentFrameIndex()];

		vkResetCommandPool(m_Device->GetHandle(), m_CurrentCommandBuffer.pool, 0);
		VkCommandBufferBeginInfo cmd_buffer_begin_info = {};
		cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VK_CHECK_RESULT(vkBeginCommandBuffer(m_CurrentCommandBuffer.buffer, &cmd_buffer_begin_info));

		auto viewportBounds = m_Swapchain->GetExtent();
		VkViewport viewport = { 0, viewportBounds.second, viewportBounds.first, -(float)viewportBounds.second, 0.0f, 1.0f };
		vkCmdSetViewport(m_CurrentCommandBuffer.buffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent = { viewportBounds.first, viewportBounds.second };
		scissor.offset = { 0, 0 };
		vkCmdSetScissor(m_CurrentCommandBuffer.buffer, 0, 1, &scissor);

		// Before rendering, we need to transition swapchain image into VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL layout,
		// so it will be optimal for rendering, since we draw stuff directly into swapchain images
		TransitionImageLayout(
			m_CurrentCommandBuffer.buffer,
			m_Swapchain->GetCurrentImage(), 
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		);

	}

	void VulkanRenderer::EndFrame()
	{
		// Right before image being presented, it should be transitioned back into VK_IMAGE_LAYOUT_PRESENT_SRC_KHR layout.
		TransitionImageLayout(
			m_CurrentCommandBuffer.buffer,
			m_Swapchain->GetCurrentImage(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		);

		vkEndCommandBuffer(m_CurrentCommandBuffer.buffer);

		VkPipelineStageFlags stagemasks[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitinfo = {};
		submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitinfo.commandBufferCount = 1;
		submitinfo.pCommandBuffers = &m_CurrentCommandBuffer.buffer;
		submitinfo.signalSemaphoreCount = 1;
		submitinfo.pSignalSemaphores = &m_Swapchain->m_Semaphores.renderComplete;
		submitinfo.waitSemaphoreCount = 1;
		submitinfo.pWaitSemaphores = &m_Swapchain->m_Semaphores.presentComplete;
		submitinfo.pWaitDstStageMask = stagemasks;

		vkQueueSubmit(m_Device->GetGraphicsTransferQueue(), 1, &submitinfo, m_Swapchain->m_Fences[m_Swapchain->m_CurrentFrameIndex]);

		m_Swapchain->EndFrame();
	}

	void VulkanRenderer::BeginRender(Ref<Image> target)
	{
		auto vk_target = RefAs<VulkanImage>(target);

		VkRenderingAttachmentInfo color_rendering_attachment = {};
		color_rendering_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		color_rendering_attachment.imageView = vk_target->GetImageView();
		color_rendering_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		color_rendering_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		color_rendering_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingAttachmentInfo depth_rendering_attachment = {};
		depth_rendering_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depth_rendering_attachment.imageView = m_Swapchain->GetDepthBuffer().view;
		depth_rendering_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_rendering_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_rendering_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		depth_rendering_attachment.clearValue = { 1.0f, 0 };

		auto renderArea = m_Swapchain->GetExtent();

		VkRenderingInfo rendering_info = {};
		rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		rendering_info.colorAttachmentCount = 1;
		rendering_info.pColorAttachments = &color_rendering_attachment;
		rendering_info.pDepthAttachment = &depth_rendering_attachment;
		rendering_info.layerCount = 1;
		rendering_info.renderArea = {
			{ 0, 0 },
			{ renderArea.first, renderArea.second }
		};

		vkCmdBeginRendering(m_CurrentCommandBuffer.buffer, &rendering_info);
	}

	void VulkanRenderer::EndRender()
	{
		vkCmdEndRendering(m_CurrentCommandBuffer.buffer);
	}

	void VulkanRenderer::ExecuteCommands(Ref<CommandBuffer> cmd_buffer)
	{
		VkPipelineStageFlags stagemasks[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		Ref<VulkanCommandBuffer> vulkan_cmd_buffer = RefAs<VulkanCommandBuffer>(cmd_buffer);

		VkCommandBuffer handle = vulkan_cmd_buffer->GetHandle();

		VkSubmitInfo submitinfo = {};
		submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitinfo.commandBufferCount = 1;
		submitinfo.pCommandBuffers = &handle;
		submitinfo.signalSemaphoreCount = 1;
		submitinfo.pSignalSemaphores = &m_Swapchain->m_Semaphores.renderComplete;
		submitinfo.waitSemaphoreCount = 1;
		submitinfo.pWaitSemaphores = &m_Swapchain->m_Semaphores.presentComplete;
		submitinfo.pWaitDstStageMask = stagemasks;

		switch (vulkan_cmd_buffer->GetType())
		{
		case CommandBufferType::GRAPHICS_TRANSFER:
			VK_CHECK_RESULT(vkQueueSubmit(m_Device->GetGraphicsTransferQueue(), 1, &submitinfo, m_Swapchain->m_Fences[m_Swapchain->m_CurrentFrameIndex]));
			break;
		case CommandBufferType::COMPUTE:
			VK_CHECK_RESULT(vkQueueSubmit(m_Device->GetComputeQueue(), 1, &submitinfo, VK_NULL_HANDLE));
			break;
		default:
			VISUS_ASSERT(false, "Attempting to execute null command buffer!");
			break;
		}
	}

	void VulkanRenderer::ClearImage(Ref<Image> image, float r, float g, float b, float a, bool now)
	{
		auto vk_image = RefAs<VulkanImage>(image);

		VkClearColorValue clear_color = {};
		clear_color.float32[0] = r;
		clear_color.float32[1] = g;
		clear_color.float32[2] = b;
		clear_color.float32[3] = a;

		VkImageSubresourceRange range = {};
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = Renderer::GetCurrentFrameIndex();
		range.layerCount = 1;

		if (now) {
			auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
			VkCommandBuffer cmd_buffer = device->GetDefaultCmdBuffer();
			VkCommandPool cmd_pool = device->GetDefaultCmdPool();

			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(cmd_buffer, &begin_info);
			vkCmdClearColorImage(cmd_buffer, vk_image->GetHandle(), VK_IMAGE_LAYOUT_GENERAL, &clear_color, 1, &range);
			vkEndCommandBuffer(cmd_buffer);

			VkFence fence;
			VkFenceCreateInfo fence_create_info = {};
			fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vkCreateFence(device->GetHandle(), &fence_create_info, nullptr, &fence);
			
			VkSubmitInfo submit_info = {};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &cmd_buffer;

			vkQueueSubmit(device->GetGraphicsTransferQueue(), 1, &submit_info, fence);
			vkWaitForFences(device->GetHandle(), 1, &fence, VK_TRUE, UINT64_MAX);

			vkDestroyFence(device->GetHandle(), fence, nullptr);
		}
		else {
			vkCmdClearColorImage(m_CurrentCommandBuffer.buffer, vk_image->GetHandle(), VK_IMAGE_LAYOUT_GENERAL, &clear_color, 1, &range);
		}
	}

	void VulkanRenderer::RenderSubmesh(Submesh* submesh, Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>> sets, const glm::mat4& transform)
	{
		auto vbo = submesh->GetVertexBuffer();
		auto ibo = submesh->GetIndexBuffer();

		auto vulkanVertexBuffer = RefAs<VulkanVertexBuffer>(vbo);
		auto vulkanIndexBuffer = RefAs<VulkanIndexBuffer>(ibo);
		auto vulkanPipeline = RefAs<VulkanPipeline>(pipeline);

		std::vector<VkDescriptorSet> vulkan_descriptor_sets(sets.size());
		for (int i = 0; i < sets.size(); i++) 
		{
			Ref<VulkanDescriptorSet> vulkan_descriptors = RefAs<VulkanDescriptorSet>(sets[i]);
			vulkan_descriptor_sets[i] = vulkan_descriptors->GetHandle();
		}

		vkCmdBindDescriptorSets(
			m_CurrentCommandBuffer.buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			vulkanPipeline->GetLayoutHandle(),
			0,
			sets.size(),
			vulkan_descriptor_sets.data(),
			0,
			0
		);

		VkDeviceSize offset = 0;

		vkCmdBindPipeline(m_CurrentCommandBuffer.buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetHandle());
		vkCmdBindVertexBuffers(m_CurrentCommandBuffer.buffer, 0, 1, vulkanVertexBuffer->GetHandle(), &offset);
		vkCmdBindIndexBuffer(m_CurrentCommandBuffer.buffer, *vulkanIndexBuffer->GetHandle(), 0, vulkanIndexBuffer->GetVulkanIndexType());

		vkCmdPushConstants(
			m_CurrentCommandBuffer.buffer,
			vulkanPipeline->GetLayoutHandle(),
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(transform),
			(const void*)&transform
		);

		vkCmdDrawIndexed(
			m_CurrentCommandBuffer.buffer,
			vulkanIndexBuffer->GetCount(),
			1,
			0,
			0,
			0
		);
	}

	void VulkanRenderer::TransitionImageLayout(VkCommandBuffer cmd_buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t arrayLayer, uint32_t layerCount)
	{
		VkImageMemoryBarrier image_memory_barrier = {};
		image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		image_memory_barrier.oldLayout = oldLayout;
		image_memory_barrier.newLayout = newLayout;
		image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.image = image;
		image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_memory_barrier.subresourceRange.baseMipLevel = 0;
		image_memory_barrier.subresourceRange.levelCount = 1;
		image_memory_barrier.subresourceRange.baseArrayLayer = arrayLayer;
		image_memory_barrier.subresourceRange.layerCount = layerCount;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			
			vkCmdPipelineBarrier(
				cmd_buffer,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &image_memory_barrier
			);
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(
				cmd_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &image_memory_barrier
			);
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
		{
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			vkCmdPipelineBarrier(
				cmd_buffer,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &image_memory_barrier
			);
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			
			vkCmdPipelineBarrier(
				cmd_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &image_memory_barrier
			);
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
			image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			vkCmdPipelineBarrier(
				cmd_buffer,
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &image_memory_barrier
			);
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

			vkCmdPipelineBarrier(
				cmd_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &image_memory_barrier
			);
		}
	}
	
	void VulkanRenderer::BlitToSwapchain(Ref<Image> image)
	{
		auto src_image = RefAs<VulkanImage>(image);
		auto resolution = m_Swapchain->GetExtent();

		VkOffset3D offset[2] = { {0, 0, 1}, {0, 0, 1} };

		VkImageBlit image_blit = {};
		image_blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_blit.srcSubresource.baseArrayLayer = Renderer::GetCurrentFrameIndex();
		image_blit.srcSubresource.layerCount = 1;
		image_blit.srcSubresource.mipLevel = 0;
		image_blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_blit.dstSubresource.baseArrayLayer = 0;
		image_blit.dstSubresource.layerCount = 1;
		image_blit.dstSubresource.mipLevel = 0;
		image_blit.srcOffsets[0] = { 0, 0, 0 };
		image_blit.srcOffsets[1] = { (int32_t)resolution.first, (int32_t)resolution.second, 1};
		image_blit.dstOffsets[0] = { 0, 0, 0 };
		image_blit.dstOffsets[1] = { (int32_t)resolution.first, (int32_t)resolution.second, 1 };

		TransitionImageLayout(
			m_CurrentCommandBuffer.buffer,
			src_image->GetHandle(),
			VK_IMAGE_LAYOUT_GENERAL,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			Renderer::GetCurrentFrameIndex()
		);

		vkCmdBlitImage(
			m_CurrentCommandBuffer.buffer, 
			src_image->GetHandle(), 
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			m_Swapchain->GetCurrentImage(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&image_blit,
			VK_FILTER_NEAREST
		);

		TransitionImageLayout(
			m_CurrentCommandBuffer.buffer,
			src_image->GetHandle(),
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_GENERAL,
			Renderer::GetCurrentFrameIndex()
		);

	}

	void VulkanRenderer::DispatchCompute(Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>> sets, uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ)
	{
		auto vk_pipeline = RefAs<VulkanPipeline>(pipeline);
		
		std::vector<VkDescriptorSet> vk_descriptor_sets;
		for (auto& set : sets) {
			vk_descriptor_sets.push_back(RefAs<VulkanDescriptorSet>(set)->GetHandle());
		}

		vkCmdBindDescriptorSets(
			m_CurrentCommandBuffer.buffer, 
			VK_PIPELINE_BIND_POINT_COMPUTE, 
			vk_pipeline->GetLayoutHandle(), 
			0, 
			vk_descriptor_sets.size(), 
			vk_descriptor_sets.data(), 
			0, 
			0
		);

		vkCmdBindPipeline(m_CurrentCommandBuffer.buffer, VK_PIPELINE_BIND_POINT_COMPUTE, vk_pipeline->GetHandle());
		vkCmdDispatch(m_CurrentCommandBuffer.buffer, workGroupX, workGroupY, workGroupZ);
	}

	void VulkanRenderer::ClearColor(float r, float b, float g, float a)
	{
		std::array<VkClearValue, 2> clear_values;
		clear_values[0].color = { {r, g, b, a} };
		clear_values[1].depthStencil = { 1.0f, 0 };

		VkRenderingAttachmentInfo color_rendering_attachment = {};
		color_rendering_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		//color_rendering_attachment.imageView = m_Swapchain->GetCurrentImage();
		color_rendering_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		color_rendering_attachment.clearValue = clear_values[0];
		color_rendering_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_rendering_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		auto renderArea = m_Swapchain->GetExtent();

		VkRenderingInfo rendering_info = {};
		rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		rendering_info.colorAttachmentCount = 1;
		rendering_info.pColorAttachments = &color_rendering_attachment;
		rendering_info.layerCount = 1;
		rendering_info.renderArea = {
			{0, 0},
			{renderArea.first, renderArea.second}
		};

		vkCmdBeginRendering(m_CurrentCommandBuffer.buffer, &rendering_info);
		vkCmdEndRendering(m_CurrentCommandBuffer.buffer);

	}
}
#include "VulkanRenderer.h"

#include <Visus/Core/Renderer.h>

#include <Visus/Platform/VulkanVertexBuffer.h>
#include <Visus/Platform/VulkanIndexBuffer.h>
#include <Visus/Platform/VulkanPipeline.h>

#include <Visus/Platform/VulkanUniformBuffer.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <Visus/Core/Renderer.h>
#include <Visus/Platform/VulkanDescriptorSet.h>

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
			m_Swapchain->GetCurrentImageView().image,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		);

	}

	void VulkanRenderer::EndFrame()
	{
		// Right before image being presented, it should be transitioned back into VK_IMAGE_LAYOUT_PRESENT_SRC_KHR layout.
		TransitionImageLayout(
			m_CurrentCommandBuffer.buffer,
			m_Swapchain->GetCurrentImageView().image,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
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

	void VulkanRenderer::BeginRender()
	{
		VkRenderingAttachmentInfo color_rendering_attachment = {};
		color_rendering_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		color_rendering_attachment.imageView = m_Swapchain->GetCurrentImageView().view;
		color_rendering_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		color_rendering_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
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
	}

	void VulkanRenderer::EndRender()
	{
		vkCmdEndRendering(m_CurrentCommandBuffer.buffer);
	}

	void VulkanRenderer::RenderMesh(Ref<VertexBuffer> vbo, Ref<IndexBuffer> ibo, Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>> sets, const glm::vec3& transform)
	{
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

		glm::mat4 model = glm::translate(glm::mat4(1.0f), transform);

		vkCmdPushConstants(
			m_CurrentCommandBuffer.buffer,
			vulkanPipeline->GetLayoutHandle(),
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(model),
			(const void*)&model
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

	void VulkanRenderer::TransitionImageLayout(VkCommandBuffer cmd_buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
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
		image_memory_barrier.subresourceRange.baseArrayLayer = 0;
		image_memory_barrier.subresourceRange.layerCount = 1;

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
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) 
		{
			image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			vkCmdPipelineBarrier(
				cmd_buffer,
				VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &image_memory_barrier
			);
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
			image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			
			vkCmdPipelineBarrier(
				cmd_buffer,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &image_memory_barrier
			);
		}
	}
	
	void VulkanRenderer::ClearColor(float r, float b, float g, float a)
	{
		VkClearValue clear_value = {};
		clear_value.color = { r, g, b, a};

		VkRenderingAttachmentInfo color_rendering_attachment = {};
		color_rendering_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		color_rendering_attachment.imageView = m_Swapchain->GetCurrentImageView().view;
		color_rendering_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		color_rendering_attachment.clearValue = clear_value;
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
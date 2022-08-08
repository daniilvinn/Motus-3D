#include "VulkanRenderer.h"

#include <Visus/Core/Renderer.h>

#include <Visus/Platform/VulkanVertexBuffer.h>
#include <Visus/Platform/VulkanIndexBuffer.h>
#include <Visus/Platform/VulkanPipeline.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace Motus3D {

	static struct GlobalRenderData {
		// All shaders MUST have Descriptor Set #0. It is reserved as renderer's scene data.
		// Currently it contains only UBO with VP matrix, but it will be extended further.
		VkDescriptorPool scene_data_pool;
		VkDescriptorSet scene_data_set;
		VkDescriptorSetLayout scene_data_set_layout;

		// VulkanUniformBuffer m_VPMatrixBuffer;
	} s_GlobalRenderData;

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

		for(int i = 0; i < renderer_config.framesInFlight; i++)
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

			m_CommandBuffers.push_back({cmd_pool, cmd_buffer});
		}

		// Descriptor pool
		std::vector<VkDescriptorPoolSize> pool_sizes = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
		};

		VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
		descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptor_pool_create_info.maxSets = 1;
		descriptor_pool_create_info.poolSizeCount = pool_sizes.size();
		descriptor_pool_create_info.pPoolSizes = pool_sizes.data();
		
		VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device->GetHandle(), &descriptor_pool_create_info, nullptr, &s_GlobalRenderData.scene_data_pool));

		// Set layout
		VkDescriptorSetLayoutBinding set_layout_binding = {};
		set_layout_binding.binding = 0;
		set_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		set_layout_binding.descriptorCount = 1;
		set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		VkDescriptorSetLayoutCreateInfo set_layout_create_info = {};
		set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		set_layout_create_info.bindingCount = 1;
		set_layout_create_info.pBindings = &set_layout_binding;

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device->GetHandle(), &set_layout_create_info, nullptr, &s_GlobalRenderData.scene_data_set_layout));

		// Descriptor Set
		VkDescriptorSetAllocateInfo set_allocate_info = {};
		set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		set_allocate_info.descriptorPool = s_GlobalRenderData.scene_data_pool;
		set_allocate_info.descriptorSetCount = 1;
		set_allocate_info.pSetLayouts = &s_GlobalRenderData.scene_data_set_layout;

		VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device->GetHandle(), &set_allocate_info, &s_GlobalRenderData.scene_data_set));

	}

	VulkanRenderer::~VulkanRenderer()
	{
		vkQueueWaitIdle(m_Device->GetGraphicsTransferQueue());
		for(auto& cmd : m_CommandBuffers)
		{
			vkDestroyCommandPool(m_Device->GetHandle(), cmd.pool, nullptr);
		}

		vkDestroyDescriptorPool(m_Device->GetHandle(), s_GlobalRenderData.scene_data_pool, nullptr);
		vkDestroyDescriptorSetLayout(m_Device->GetHandle(), s_GlobalRenderData.scene_data_set_layout, nullptr);

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
		scissor.extent = {viewportBounds.first, viewportBounds.second};
		scissor.offset = { 0, 0 };
		vkCmdSetScissor(m_CurrentCommandBuffer.buffer, 0, 1, &scissor);

		// Before rendering, we need to transition swapchain image into VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL layout,
		// so it will be optimal for rendering, since we draw stuff directly into swapchain images
		const VkImageMemoryBarrier image_memory_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.image = m_Swapchain->GetCurrentImageView().image,
			.subresourceRange = {
			  .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			  .baseMipLevel = 0,
			  .levelCount = 1,
			  .baseArrayLayer = 0,
			  .layerCount = 1,
			}
		};

		vkCmdPipelineBarrier(
			m_CurrentCommandBuffer.buffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
			0,
			0,
			nullptr,
			0,
			nullptr,
			1, // imageMemoryBarrierCount
			&image_memory_barrier // pImageMemoryBarriers
		);

	}

	void VulkanRenderer::EndFrame()
	{
		// Right before image being presented, it should be transitioned back into VK_IMAGE_LAYOUT_PRESENT_SRC_KHR layout.
		const VkImageMemoryBarrier image_memory_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.image = m_Swapchain->GetCurrentImageView().image,
			.subresourceRange = {
			  .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			  .baseMipLevel = 0,
			  .levelCount = 1,
			  .baseArrayLayer = 0,
			  .layerCount = 1,
			}
		};

		vkCmdPipelineBarrier(
			m_CurrentCommandBuffer.buffer,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
			0,
			0,
			nullptr,
			0,
			nullptr,
			1, // imageMemoryBarrierCount
			&image_memory_barrier // pImageMemoryBarriers
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

	void VulkanRenderer::RenderMesh(Ref<VertexBuffer> vbo, Ref<IndexBuffer> ibo, Ref<Pipeline> pipeline, const glm::mat4 vp, const glm::vec3& transform)
	{
		auto vulkanVertexBuffer = RefAs<VulkanVertexBuffer>(vbo);
		auto vulkanIndexBuffer = RefAs<VulkanIndexBuffer>(ibo);
		auto vulkanPipeline = RefAs<VulkanPipeline>(pipeline);

		VkDeviceSize offset = 0;

		vkCmdBindPipeline(m_CurrentCommandBuffer.buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetHandle());
		vkCmdBindVertexBuffers(m_CurrentCommandBuffer.buffer, 0, 1, vulkanVertexBuffer->GetHandle(), &offset);
		vkCmdBindIndexBuffer(m_CurrentCommandBuffer.buffer, *vulkanIndexBuffer->GetHandle(), 0, vulkanIndexBuffer->GetVulkanIndexType());

		glm::mat4 model = glm::translate(glm::mat4(1.0f), transform);

		glm::mat4 constants[] = { vp, model };

		vkCmdPushConstants(
			m_CurrentCommandBuffer.buffer,
			vulkanPipeline->GetLayoutHandle(),
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(constants),
			(const void*)constants
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
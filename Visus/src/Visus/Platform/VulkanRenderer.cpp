#include "VulkanRenderer.h"

#include <Visus/Core/Renderer.h>

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
	}

	VulkanRenderer::~VulkanRenderer()
	{
		vkQueueWaitIdle(m_Device->GetGraphicsTransferQueue());
		for(auto& cmd : m_CommandBuffers)
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
		vkResetCommandPool(m_Device->GetHandle(), m_CommandBuffers[m_Swapchain->GetCurrentFrameIndex()].pool, 0);
		VkCommandBufferBeginInfo cmd_buffer_begin_info = {};
		cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffers[m_Swapchain->GetCurrentFrameIndex()].buffer, &cmd_buffer_begin_info));
	}

	void VulkanRenderer::EndFrame()
	{
		vkEndCommandBuffer(m_CommandBuffers[m_Swapchain->GetCurrentFrameIndex()].buffer);
		VkPipelineStageFlags stagemasks[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitinfo = {};
		submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitinfo.commandBufferCount = 1;
		submitinfo.pCommandBuffers = &m_CommandBuffers[m_Swapchain->GetCurrentFrameIndex()].buffer;
		submitinfo.signalSemaphoreCount = 1;
		submitinfo.pSignalSemaphores = &m_Swapchain->m_Semaphores.renderComplete;
		submitinfo.waitSemaphoreCount = 1;
		submitinfo.pWaitSemaphores = &m_Swapchain->m_Semaphores.presentComplete;
		submitinfo.pWaitDstStageMask = stagemasks;

		VK_CHECK_RESULT(vkQueueSubmit(m_Device->GetGraphicsTransferQueue(), 1, &submitinfo, m_Swapchain->m_Fences[m_Swapchain->m_CurrentFrameIndex]));
		m_Swapchain->EndFrame();
	}

	void VulkanRenderer::ClearColor(float r, float b, float g, float a)
	{
		VkClearValue clear_value = {};
		clear_value.color = { r, g, b, a};

		VkRenderPassBeginInfo render_pass_begin_info = {};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_begin_info.renderPass = m_Swapchain->GetRenderPass();
		render_pass_begin_info.framebuffer = m_Swapchain->GetCurrentFramebuffer();
		render_pass_begin_info.clearValueCount = 1;
		render_pass_begin_info.pClearValues = &clear_value;
		render_pass_begin_info.renderArea.offset = { 0, 0 };
		render_pass_begin_info.renderArea.extent = { m_Swapchain->GetExtent().first, m_Swapchain->GetExtent().second };

		vkCmdBeginRenderPass(m_CommandBuffers[m_Swapchain->GetCurrentFrameIndex()].buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdEndRenderPass(m_CommandBuffers[m_Swapchain->GetCurrentFrameIndex()].buffer);

	}
}
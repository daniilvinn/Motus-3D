#pragma once

#include <Visus/Core/RendererAPI.h>
#include <Visus/Platform/VulkanGraphicsContext.h>

#include <Visus/Core/DescriptorSet.h>
#include <Visus/Core/Pipeline.h>
#include <Visus/Core/Image.h>
#include <Visus/Core/Model.h>

#include <glm/glm.hpp>

namespace Motus3D {
	class VulkanRenderer : public RendererAPI
	{
	public:
		VulkanRenderer();
		~VulkanRenderer();

		void OnWindowResize(uint32_t width, uint32_t height, bool vsync) override;

		Ref<GraphicsContext> GetContext() override { return m_GraphicsContext; };
		uint8_t GetCurrentFrameIndex() const override { return m_Swapchain->GetCurrentFrameIndex(); }

		void BeginFrame() override;
		void EndFrame() override;
		void BeginRender() override;
		void EndRender() override;
		void BlitToSwapchain(Ref<Image> image) override;

		void ExecuteCommands(Ref<CommandBuffer> cmd_buffer) override;

		// Clears render target (now only current swapchain image) with specified color.
		void ClearColor(float r, float b, float g, float a) override;
		void RenderSubmesh(Submesh* submesh, Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>> sets, const glm::mat4& transform) override;

		void DispatchCompute(Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>> sets, uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ) override;

		// It is assumed that command buffer recording is already started and will be ended manually.
		static void TransitionImageLayout(VkCommandBuffer cmd_buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t arrayLayer = 0, uint32_t layerCount = 1);



	private:
		Ref<VulkanGraphicsContext> m_GraphicsContext;
		Ref<VulkanDevice> m_Device;
		Ref<VulkanSwapchain> m_Swapchain;

		struct CommandBufferAndPool
		{
			VkCommandPool pool;
			VkCommandBuffer buffer;
		};
		std::vector<CommandBufferAndPool> m_CommandBuffers;

		CommandBufferAndPool m_CurrentCommandBuffer;

	};
}
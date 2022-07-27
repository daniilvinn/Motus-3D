#pragma once

#include <Visus/Core/RendererAPI.h>
#include <Visus/Platform/VulkanGraphicsContext.h>

namespace Motus3D {
	class VulkanRenderer : public RendererAPI
	{
	public:
		VulkanRenderer();
		~VulkanRenderer();

		void OnWindowResize(uint32_t width, uint32_t height, bool vsync) override;

		Ref<GraphicsContext> GetContext() override { return m_GraphicsContext; };

		void BeginFrame() override;
		void EndFrame() override;
		void BeginRender() override;
		void EndRender() override;

		void ClearColor(float r, float b, float g, float a) override;
		void RenderMesh(Ref<VertexBuffer> vbo, Ref<IndexBuffer> ibo, Ref<Pipeline> pipeline) override;

	private:
		Ref<VulkanGraphicsContext> m_GraphicsContext;
		Ref<VulkanDevice> m_Device;
		Ref<VulkanSwapchain> m_Swapchain;

		struct CommandBuffer
		{
			VkCommandPool pool;
			VkCommandBuffer buffer;
		};
		std::vector<CommandBuffer> m_CommandBuffers;

	};
}
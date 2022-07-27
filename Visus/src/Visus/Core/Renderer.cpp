#include "Renderer.h"

#include <Visus/Platform/VulkanRenderer.h>

namespace Motus3D
{
	Ref<RendererAPI> Renderer::s_RendererAPI = nullptr;
	RendererConfiguration Renderer::s_Configuration;

	void Renderer::Init(RendererConfiguration configuration)
	{
		s_Configuration = configuration;
		s_RendererAPI = CreateRef<VulkanRenderer>();
	}

	void Renderer::Shutdown()
	{
		s_RendererAPI.reset();
	}

	Ref<GraphicsContext> Renderer::GetContext()
	{
		return s_RendererAPI->GetContext();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height, bool vsync)
	{
		s_RendererAPI->OnWindowResize(width, height, vsync);
	}

	void Renderer::BeginFrame()
	{
		s_RendererAPI->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		s_RendererAPI->EndFrame();
	}

	void Renderer::BeginRender()
	{
		s_RendererAPI->BeginRender();
	}

	void Renderer::EndRender()
	{
		s_RendererAPI->EndRender();
	}

	void Renderer::ClearColor(float r, float g, float b, float a)
	{
		s_RendererAPI->ClearColor(r, g, b, a);
	}

	void Renderer::Submit(Ref<VertexBuffer> vbo, Ref<IndexBuffer> ibo, Ref<Pipeline> pipeline)
	{
		s_RendererAPI->RenderMesh(vbo, ibo, pipeline);
	}

}

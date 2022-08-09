#include "Renderer.h"

#include <Visus/Platform/VulkanRenderer.h>

namespace Motus3D
{
	Ref<RendererAPI> Renderer::s_RendererAPI = nullptr;
	RendererConfiguration Renderer::s_Configuration;
	Renderer::SceneData Renderer::m_SceneData = Renderer::SceneData();

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

	float Renderer::GetDeltaTime()
	{
		MT_CORE_ASSERT(false, "Method not implemented");
		return 0.0f;
	}

	void Renderer::BeginFrame()
	{
		s_RendererAPI->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		s_RendererAPI->EndFrame();
	}

	void Renderer::BeginScene(SceneData data)
	{
		m_SceneData = data;
		s_RendererAPI->BeginRender();
	}

	void Renderer::EndScene()
	{
		s_RendererAPI->EndRender();
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

	void Renderer::Submit(Ref<VertexBuffer> vbo, Ref<IndexBuffer> ibo, Ref<Pipeline> pipeline, const glm::vec3& transform)
	{
		s_RendererAPI->RenderMesh(vbo, ibo, pipeline, m_SceneData.camera->GetViewProjectionMatrix(), transform);
	}

}

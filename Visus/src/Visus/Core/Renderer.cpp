#include "Renderer.h"

#include <Visus/Platform/VulkanRenderer.h>

namespace Motus3D
{
	Ref<RendererAPI> Renderer::s_RendererAPI = nullptr;
	RendererConfiguration Renderer::s_Configuration;
	std::vector<Ref<DescriptorSet>> Renderer::s_SceneDataDescriptorSets;
	std::vector<Ref<UniformBuffer>> Renderer::s_CameraDataBuffers;
		
	void Renderer::Init(RendererConfiguration configuration)
	{
		s_Configuration = configuration;
		s_RendererAPI = CreateRef<VulkanRenderer>();

		// Setting up descriptor sets
		for (int i = 0; i < s_Configuration.framesInFlight; i++) {
			s_SceneDataDescriptorSets.push_back(DescriptorSet::Create({
				{ 0, ResourceType::UBO, ShaderStage::VERTEX, 1 }
			}));

			s_CameraDataBuffers.push_back(UniformBuffer::Create(sizeof(glm::mat4) * 3, 0));

			s_SceneDataDescriptorSets[i]->UpdateDescriptor(0, 0, 0, s_CameraDataBuffers[i], 0);
		}
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
		s_RendererAPI->BeginRender();

		glm::mat4 matrices[3] = 
		{
			data.camera->GetViewProjectionMatrix(),
			data.camera->GetProjectionMatrix(),
			data.camera->GetViewMatrix()
		};

		s_CameraDataBuffers[s_RendererAPI->GetCurrentFrameIndex()]->SetData(matrices, sizeof(matrices));

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

	// Temporar solution. Renderer shouldn't own ANY descriptor set. To be moved to Sandbox2D.
	void Renderer::Submit(Ref<VertexBuffer> vbo, Ref<IndexBuffer> ibo, Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>> sets, const glm::vec3& transform)
	{
		sets.insert(sets.begin(), s_SceneDataDescriptorSets[0]);
		s_RendererAPI->RenderMesh(
			vbo,
			ibo,
			pipeline,
			sets,
			transform
		);
	}

}

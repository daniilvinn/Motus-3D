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
		for (int i = 0; i < s_Configuration.framesInFlight; i++)
		{
			s_SceneDataDescriptorSets[i].reset();
			s_CameraDataBuffers[i].reset();
		}
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

	uint32_t Renderer::GetCurrentFrameIndex()
	{
		return s_RendererAPI->GetCurrentFrameIndex();
	}

	void Renderer::BeginFrame()
	{
		s_RendererAPI->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		s_RendererAPI->EndFrame();
	}

	void Renderer::BeginScene(SceneData data, Ref<Image> target)
	{
		s_RendererAPI->BeginRender(target);

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

	void Renderer::BlitToSwapchain(Ref<Image> image)
	{
		s_RendererAPI->BlitToSwapchain(image);
	}

	void Renderer::BeginRender(Ref<Image> target)
	{
		s_RendererAPI->BeginRender(target);
	}

	void Renderer::EndRender()
	{
		s_RendererAPI->EndRender();
	}

	void Renderer::ClearColor(float r, float g, float b, float a)
	{
		s_RendererAPI->ClearColor(r, g, b, a);
	}

	void Renderer::ClearImage(Ref<Image> image, float r, float g, float b, float a, bool now /*= true*/)
	{
		s_RendererAPI->ClearImage(image, r, g, b, a, now);
	}

	// Temporary solution. Renderer shouldn't own ANY descriptor set. To be moved to Sandbox2D.
	void Renderer::Submit(Submesh* submesh, Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>> sets, const glm::mat4& transform)
	{
		sets.insert(sets.begin(), s_SceneDataDescriptorSets[0]);
		s_RendererAPI->RenderSubmesh(
			submesh,
			pipeline,
			sets,
			transform
		);
	}

	void Renderer::Dispatch(Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>> sets, uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ)
	{
		s_RendererAPI->DispatchCompute(pipeline, sets, workGroupX, workGroupY, workGroupZ);
	}

}

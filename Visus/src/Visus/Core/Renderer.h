#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/VertexBuffer.h>
#include <Visus/Core/IndexBuffer.h>
#include <Visus/Core/Pipeline.h>

#include <Visus/Core/Camera.h>

#include <Visus/Core/RendererAPI.h>

#include <Visus/Core/DescriptorSet.h>

namespace Motus3D {
	struct VISUS_API RendererConfiguration
	{
		void* windowHandle;
		uint8_t framesInFlight;
	};

	class VISUS_API Renderer
	{
	public:
		struct SceneData {
			Ref<Camera> camera;
		};
	public:
		static void Init(RendererConfiguration configuration);
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height, bool vsync = true);

		static Ref<GraphicsContext> GetContext();
		static RendererConfiguration GetConfiguration() { return s_Configuration; }
		static float GetDeltaTime();

		static void BeginFrame();
		static void EndFrame();
		static void BeginScene(SceneData data);
		static void EndScene();


		static void ClearColor(float r, float g, float b, float a);
		static void Submit(
			Ref<VertexBuffer> vbo, 
			Ref<IndexBuffer> ibo, 
			Ref<Pipeline> pipeline, 
			std::vector<Ref<DescriptorSet>> sets,
			const glm::vec3& transform
		);

	private:
		static void BeginRender();
		static void EndRender();

	private:
		static Ref<RendererAPI> s_RendererAPI;
		static RendererConfiguration s_Configuration;

		// Scene data descriptor sets and buffers, images with its data
		static std::vector<Ref<DescriptorSet>> s_SceneDataDescriptorSets;
		static std::vector<Ref<UniformBuffer>> s_CameraDataBuffers;

		static float m_DeltaTime;
	};
}
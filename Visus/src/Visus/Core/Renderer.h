#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/VertexBuffer.h>
#include <Visus/Core/IndexBuffer.h>
#include <Visus/Core/Pipeline.h>

#include <Visus/Core/RendererAPI.h>

namespace Motus3D {
	struct VISUS_API RendererConfiguration
	{
		void* windowHandle;
		uint8_t framesInFlight;
	};

	class VISUS_API Renderer
	{
	public:
		static void Init(RendererConfiguration configuration);
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height, bool vsync = true);

		static Ref<GraphicsContext> GetContext();
		static RendererConfiguration GetConfiguration() { return s_Configuration; }

		static void BeginFrame();
		static void EndFrame();
		static void BeginRender();
		static void EndRender();

		static void ClearColor(float r, float g, float b, float a);
		static void Submit(Ref<VertexBuffer> vbo, Ref<IndexBuffer> ibo, Ref<Pipeline> pipeline);

	private:
		static Ref<RendererAPI> s_RendererAPI;
		static RendererConfiguration s_Configuration;
	};
}
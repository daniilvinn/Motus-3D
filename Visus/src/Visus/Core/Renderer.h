#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/RendererAPI.h>

namespace Visus {
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

		static Ref<GraphicsContext> GetContext();
		static RendererConfiguration GetConfiguration() { return m_Configuration; }

		static void OnWindowResize(uint32_t width, uint32_t height, bool vsync = true);

		static void BeginFrame();
		static void EndFrame();
		static void ClearColor(float r, float g, float b, float a);

	private:
		static Ref<RendererAPI> s_RendererAPI;
		static RendererConfiguration m_Configuration;
	};
}
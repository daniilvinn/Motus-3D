#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/GraphicsContext.h>

namespace Visus {
	class VISUS_API RendererAPI
	{
	public:
		virtual void OnWindowResize(uint32_t width, uint32_t height, bool vsync) = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual Ref<GraphicsContext> GetContext() = 0;
		virtual void ClearColor(float r, float b, float g, float a) = 0;

	};
}
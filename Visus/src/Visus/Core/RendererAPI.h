#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/GraphicsContext.h>

#include <Visus/Core/VertexBuffer.h>
#include <Visus/Core/IndexBuffer.h>
#include <Visus/Core/Pipeline.h>

namespace Motus3D {
	class VISUS_API RendererAPI
	{
	public:
		virtual Ref<GraphicsContext> GetContext() = 0;
		virtual void OnWindowResize(uint32_t width, uint32_t height, bool vsync) = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void BeginRender() = 0;
		virtual void EndRender() = 0;

		virtual void ClearColor(float r, float b, float g, float a) = 0;
		virtual void RenderMesh(Ref<VertexBuffer> vbo, Ref<IndexBuffer> ibo, Ref<Pipeline> pipeline) = 0;

	};
}
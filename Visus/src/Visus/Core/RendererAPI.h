#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/GraphicsContext.h>
#include <Visus/Core/VertexBuffer.h>
#include <Visus/Core/IndexBuffer.h>
#include <Visus/Core/Pipeline.h>
#include <Visus/Core/DescriptorSet.h>
#include <Visus/Core/Model.h>
#include <Visus/Core/CommandBuffer.h>

#include <glm/glm.hpp>

namespace Motus3D {
	class VISUS_API RendererAPI
	{
	public:
		virtual Ref<GraphicsContext> GetContext() = 0;
		virtual void OnWindowResize(uint32_t width, uint32_t height, bool vsync) = 0;

		virtual uint8_t GetCurrentFrameIndex() const = 0;
		virtual uint64_t GetGPUMemoryUsage() const = 0;

		// Other helping methods
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void BeginRender(Ref<Image> target, bool useDepth = true) = 0;
		virtual void EndRender() = 0;
		virtual void BlitToSwapchain(Ref<Image> image) = 0;
		virtual void RenderImGui() = 0;

		virtual void ExecuteCommands(Ref<CommandBuffer> cmd_buffer) = 0;

		virtual void SetupImGuiRenderTarget(Ref<Image> target) = 0;

		// Actual rendering
		virtual void ClearImage(Ref<Image> image, float r, float b, float g, float a, bool now) = 0;
		virtual void ClearColor(float r, float b, float g, float a) = 0;
		virtual void RenderSubmesh(Submesh* submesh, Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>> sets, const glm::mat4& transform) = 0;

		// Compute
		virtual void DispatchCompute(Ref<Pipeline>, std::vector<Ref<DescriptorSet>> sets, uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ) = 0;

		
	};
}
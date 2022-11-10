#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/VertexBuffer.h>
#include <Visus/Core/IndexBuffer.h>
#include <Visus/Core/Pipeline.h>
#include <Visus/Core/Camera.h>
#include <Visus/Core/RendererAPI.h>
#include <Visus/Core/DescriptorSet.h>
#include <Visus/Core/Model.h>

namespace Motus3D {
	struct VISUS_API RendererConfiguration
	{
		void* windowHandle;
		uint8_t framesInFlight;
	};

	enum class VISUS_API QueueFamily : uint8_t {
		GRAPHICS_TRANSFER,
		COMPUTE
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

		static void OnWindowResize(uint32_t width, uint32_t height, bool vsync = false);

		static Ref<GraphicsContext> GetContext();
		static RendererConfiguration GetConfiguration() { return s_Configuration; }
		static float GetDeltaTime();
		static uint32_t GetCurrentFrameIndex();

		static void BeginFrame();
		static void EndFrame();
		static void BeginScene(SceneData data, Ref<Image> target);
		static void EndScene();
		static void BlitToSwapchain(Ref<Image> image);


		//static void ExecuteCommands(Ref<CommandBuffer> cmd_buffer, QueueFamily family);

		static void ClearColor(float r, float g, float b, float a);
		static void ClearImage(Ref<Image> image, float r, float g, float b, float a, bool now = true);
		static void Submit(
			Submesh* submesh,
			Ref<Pipeline> pipeline, 
			std::vector<Ref<DescriptorSet>> sets,
			const glm::mat4& transform
		);

		/*
		@brief Launches compute pipeline on main general-purpose GPU queue, thus commands will be executed in order to previous recorded commands.
		
		@param[in] pipeline - MUST be compute pipeline. Will be launched on command buffer execution.
		@param[in] sets - array of descriptor sets (static data), that points to data on GPU.
		@param[in] workGroupXYZ - work group dimensions. Can be treated as image resolution or number of compute pipeline invocation.

		*/
		static void Dispatch(Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>> sets, uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ);

		/* 
		@brief	Launches compute pipeline on separate GPU queue,
				which is dedicated for compute commands.

		Pipeline will be launched asynchronously with main general-purpose queue, that is able to execute graphics, transfer and compute commands.

		@param[in] pipeline - MUST be compute pipeline. Will be launched on command buffer execution.
		@param[in] sets - array of descriptor sets (static data), that points to data on GPU.
		@param[in] workGroupXYZ - work group dimensions. Can be treated as image resolution or number of compute pipeline invocation.

		*/
		static void DispatchAsync(Ref<Pipeline> pipeline, std::vector<Ref<DescriptorSet>>, uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ);

		static void BeginRender(Ref<Image> target);
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
#include <Scene/Core/Macros.h>

#include <Visus.h>
#include <Visus/Core/CommandBuffer.h>

namespace Motus3D {

	struct MT_SCENE_API SceneData 
	{

	};

	class MT_SCENE_API SceneRenderer {
	public:
		SceneRenderer();
		~SceneRenderer();

		void BeginScene(const SceneData& data);
		void EndScene();

		//void Render();

	private:
		// Main pass, renders geometry with textures
		void ColorPass();

		// Apply HDR, gamma correction and skybox
		void PostProcessingPass();

	private:
		Ref<CommandBuffer> m_RenderCommandBuffer;
		std::vector<Ref<Image>> m_RenderTargets; // There should be as many render targets as frames-in-flight

		std::vector<Ref<Pipeline>> m_PostProcessingChain;

		Ref<Pipeline> m_TexturePipeline;
		Ref<Pipeline> m_SkyboxPipeline;
		Ref<Pipeline> m_GammaHDRPipeline;
		Ref<Pipeline> m_VignettePipeline;

		// TODO:
		//Ref<CommandBuffer> m_RenderCommandBuffer;

	};

}
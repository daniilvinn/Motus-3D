#include <Motus3D.h>

#define GLM_FORCE_SSE2
#define GLM_FORCE_ALIGNED
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace Motus3D;

class TestLayer : public Motus3D::Layer
{
public:
	void OnUpdate() override
	{
		OnCameraUpdate();
		m_RenderTargetSet->UpdateDescriptor(0, m_TestRenderTarget);

		if (Input::KeyPressed(KeyCode::KEY_UP)) {
			m_Acceleration.y = 0.0f;
			m_Acceleration.y += 0.001f;
			m_AccelerationMagnitude.y = 1.0f;
		};
		if (Input::KeyPressed(KeyCode::KEY_LEFT)) {
			m_Acceleration.x = 0.0f;
			m_Acceleration.x -= 0.001f;
			m_AccelerationMagnitude.x = 1.0f;
		};
		if (Input::KeyPressed(KeyCode::KEY_DOWN)) {
			m_Acceleration.y = 0.0f;
			m_Acceleration.y -= 0.001f;
			m_AccelerationMagnitude.y = 1.0f;
		};
		if (Input::KeyPressed(KeyCode::KEY_RIGHT)) {
			m_Acceleration.x = 0.0f;
			m_Acceleration.x += 0.001f;
			m_AccelerationMagnitude.x = 1.0f;
		};

		m_MeshPosition.x += m_Acceleration.x * m_AccelerationMagnitude.x;
		m_MeshPosition.y += m_Acceleration.y * m_AccelerationMagnitude.y;
		m_AccelerationMagnitude -= 0.001f;
		if (m_AccelerationMagnitude.x < 0.0f) m_AccelerationMagnitude.x = 0.0f;
		if (m_AccelerationMagnitude.y < 0.0f) m_AccelerationMagnitude.y = 0.0f;

		// Begin rendering scene
		Renderer::ClearImage(m_TestRenderTarget, 0.0f, 0.0f, 0.0f, 0.0f, false);
		Renderer::BeginScene({ RefAs<Camera>(m_Camera) }, m_TestRenderTarget);
		Submesh* skybox_mesh = m_Skybox.GetSubmesh();
		Renderer::Submit(skybox_mesh, m_SkyboxPipeline, { m_Skybox.GetDescriptorSet() }, glm::rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
		
		std::vector<Submesh> model_submeshes = m_Model.GetSubmeshes();

		glm::mat4 transform = glm::translate(m_MeshPosition) * glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)) * glm::rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		for (int i = 0; i < model_submeshes.size(); i++)
		{
			Renderer::Submit(&model_submeshes[3], m_Pipeline, { m_DescriptorSets[3] }, transform);
		}

		Renderer::EndScene();

		Renderer::Dispatch(m_PipelineFilmGrain, { m_RenderTargetSet }, (1600 / 16) + 1, (900 / 16) + 1, 1);
		Renderer::Dispatch(m_ToneMapping, { m_RenderTargetSet, m_PPSet }, (1600 / 16) + 1, (900 / 16) + 1, 1);
		Renderer::BlitToSwapchain(m_TestRenderTarget);
	};




	void OnAttach() override
	{
		Ref<Shader> main_shader = Shader::Create("texture.glsl");
		Ref<Shader> skybox_shader = Shader::Create("skybox.glsl");
		Ref<Shader> test_compute = Shader::Create("pp_gamma_hdr.glsl");
		Ref<Shader> film_grain = Shader::Create("pp_film_grain.glsl");

		m_TestRenderTarget = Image::Create(ImageUsage::RENDER_TARGET_HDR);

		VertexBufferLayout bufferLayout({
			{ "aPos", ShaderDataType::FLOAT3 },
			{ "aTexCoord", ShaderDataType::FLOAT2 },
			{ "aNormal", ShaderDataType::FLOAT3 },
		});

		VertexBufferLayout skybox_buffer_layout({
			{ "aPos", ShaderDataType::FLOAT3 }
		});

		m_Pipeline = Pipeline::Create({
			main_shader,
			PipelineExecutionModel::GRAPHICS,
			"Main",
			&bufferLayout,
			PolygonMode::FILL,
			CullMode::BACK,
			true
		});

		m_SkyboxPipeline = Pipeline::Create({
			skybox_shader,
			PipelineExecutionModel::GRAPHICS,
			"Skybox",
			&skybox_buffer_layout,
			PolygonMode::FILL,
			CullMode::NONE,
			false
		});

		m_ToneMapping = Pipeline::Create({
			test_compute,
			PipelineExecutionModel::COMPUTE,
			"TestCompute"
		});

		m_PipelineFilmGrain = Pipeline::Create({
			film_grain,
			PipelineExecutionModel::COMPUTE,
			"Film grain"
		});

		m_Camera = CreateRef<Camera3D>();
		m_Camera->SetProjection(80.0f, 1.6f / 0.9f, 0.01f, 100.0f);
		m_Camera->SetSensivity(0.1f);

		// Physics
		m_MeshPosition = glm::vec3(0.0f, 0.3f, -3.0f);
		m_Acceleration = glm::vec3(0.0f);
		m_AccelerationMagnitude = glm::vec3(0.0f);

		m_Model.Load("assets/models/solarsys/scene.gltf");

		m_Sampler = Sampler::Create(
			{
				SamplerFilter::LINEAR,
				SamplerFilter::LINEAR,
				MipmapMode::LINEAR,
				SamplerAddressMode::REPEAT,
				16.0f
			}
		);

		auto model_submeshes = m_Model.GetSubmeshes();
		m_DescriptorSets.resize(model_submeshes.size());

		for (int i = 0; i < model_submeshes.size(); i++) {
			m_DescriptorSets[i] = DescriptorSet::Create({
				{
					0,
					ResourceType::IMAGE,
					ShaderStage::FRAGMENT,
					1
				}
			});
			m_DescriptorSets[i]->UpdateDescriptor(0, model_submeshes[i].GetTexture(), m_Sampler);
		}

		m_Skybox.Load("assets/cubemaps/space", m_Sampler);

		// Post Processing
		m_PPData = UniformBuffer::Create(8, 1);
		float pp_data[] = { 2.2f, 0.8f };
		m_PPData->SetData(pp_data, sizeof(pp_data));

		m_RenderTargetSet = DescriptorSet::Create({
			{ 0, ResourceType::STORAGE_IMAGE, ShaderStage::COMPUTE, 1 },
		});
		m_PPSet = DescriptorSet::Create({
			{ 1, ResourceType::UBO, ShaderStage::COMPUTE, 1 }, 
		});
		m_RenderTargetSet->UpdateDescriptor(0, m_TestRenderTarget);
		m_PPSet->UpdateDescriptor(1, 0, 0, m_PPData);

		MT_LOG_TRACE("Test layer attached");
	};

	void OnDetach() override
	{
		MT_LOG_INFO("~Test layer on detach~");
		m_Pipeline.reset();
		m_SkyboxPipeline.reset();
		m_Model.Release();
		for (auto& set : m_DescriptorSets) {
			set->Release();
		}
		m_Skybox.Release();
		m_Sampler->Destroy();
		MT_LOG_TRACE("Test layer detached");
	};

	void OnEvent(Event& e) override 
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizedEvent>(MT_BIND_EVENT_FUNCTION(TestLayer::OnWindowResize));
		dispatcher.Dispatch<MouseMovedEvent>(MT_BIND_EVENT_FUNCTION(TestLayer::OnMouseMoved));
	};

	void OnCameraUpdate() {
		
		glm::vec3 cameraMovementVector(0.0f);

		if (Input::KeyPressed(KeyCode::KEY_W)) cameraMovementVector.z += 0.01f;
		if (Input::KeyPressed(KeyCode::KEY_S)) cameraMovementVector.z -= 0.01f;
		if (Input::KeyPressed(KeyCode::KEY_A)) cameraMovementVector.x -= 0.01f;
		if (Input::KeyPressed(KeyCode::KEY_D)) cameraMovementVector.x += 0.01f;
		if (Input::KeyPressed(KeyCode::KEY_SPACE)) cameraMovementVector.y += 0.01f;
		if (Input::KeyPressed(KeyCode::KEY_LEFT_SHIFT)) cameraMovementVector.y -= 0.01f;

		m_Camera->Move(cameraMovementVector);

		if (Input::KeyPressed(KeyCode::KEY_Q)) m_Camera->Rotate(-0.03f, 0.0f, 0.0f, false);
		if (Input::KeyPressed(KeyCode::KEY_E)) m_Camera->Rotate(0.03f, 0.0f, 0.0f, false);

	};

private:

	bool OnWindowResize(WindowResizedEvent& e) 
	{
		m_Camera->SetProjection(65.0f, (float)e.width / (float)e.height, 0.1f, 100.0f);
		m_TestRenderTarget->Invalidate();
		return false;
	}

	bool OnMouseMoved(MouseMovedEvent& e) 
	{
		// "Encapsulate" these variables in this function
		static bool firstMouseMovement = true;
		static float lastPositionX = 0.0f;
		static float lastPositionY = 0.0f;
		if (firstMouseMovement) {
			lastPositionX = e.xpos;
			lastPositionY = e.ypos;
		}

		firstMouseMovement = false;

		// TODO: figure out why xoffset requires "-" sign in equation.
		m_Camera->Rotate(-(lastPositionX - e.xpos), (lastPositionY - e.ypos), 0.0f, true);

		lastPositionX = e.xpos;
		lastPositionY = e.ypos;

		return false;
	}

private:

	// Static data
	Ref<Pipeline> m_Pipeline;
	Ref<Pipeline> m_SkyboxPipeline;
	Ref<Pipeline> m_ToneMapping;
	Ref<Pipeline> m_PipelineFilmGrain;

	Model m_Model;
	std::vector<Ref<DescriptorSet>> m_DescriptorSets;
	Ref<Image> m_Texture;
	Ref<Image> m_EnvTexture;
	Ref<Sampler> m_Sampler;


	// Post processing
	Ref<DescriptorSet> m_RenderTargetSet;
	Ref<DescriptorSet> m_PPSet;

	Ref<Image> m_TestRenderTarget;
	Ref<UniformBuffer> m_ImageData;
	Ref<UniformBuffer> m_PPData;

	// Skybox
	Skybox m_Skybox;
	
	// Dynamic data
	Ref<Camera3D> m_Camera;

	// Physics
	glm::vec3 m_MeshPosition;
	glm::vec3 m_Acceleration;
	glm::vec3 m_AccelerationMagnitude;

};

class Sandbox2D : public Motus3D::Application
{
public:
	Sandbox2D()
	{
		AddLayer(new TestLayer());
	};
	~Sandbox2D() override
	{
	};
};

Motus3D::Scope<Motus3D::Application> CreateApplication()
{
	return Motus3D::CreateScope<Sandbox2D>();
}
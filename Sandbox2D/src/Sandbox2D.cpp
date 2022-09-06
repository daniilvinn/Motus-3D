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
		Renderer::BeginScene({ RefAs<Camera>(m_Camera) });
		std::vector<Submesh> rex_submeshes = m_Model.GetSubmeshes();
		std::vector<Submesh> env_submeshes = m_EnvironmentModel.GetSubmeshes();

		glm::mat4 transform = glm::translate(m_MeshPosition) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f));

		for (int i = 0; i < rex_submeshes.size(); i++)
		{
			Renderer::Submit(&rex_submeshes[i], m_Pipeline, { m_DescriptorSet }, transform);
		}

		transform = glm::rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(10.0f, 10.0f, 10.0f));

		for (int i = 0; i < env_submeshes.size(); i++)
		{
			Renderer::Submit(&env_submeshes[i], m_Pipeline, { m_EnvDescriptorSet }, transform);
		}
		Renderer::EndScene();
		// End rendering scene and executing work on GPU
	};

	void OnAttach() override
	{
		m_Shader = Shader::Create("texture.glsl");
		VertexBufferLayout bufferLayout({
			{ "aPos", ShaderDataType::FLOAT3 },
			{ "aTexCoord", ShaderDataType::FLOAT2 },
			{ "aNormal", ShaderDataType::FLOAT3 },
		});

		m_Pipeline = Pipeline::Create({
			m_Shader,
			bufferLayout,
			PolygonMode::FILL
		});

		m_Camera = CreateRef<Camera3D>();
		m_Camera->SetProjection(80.0f, 1.6f / 0.9f, 0.01f, 100.0f);
		m_Camera->SetSensivity(0.1f);

		// Physics
		m_MeshPosition = glm::vec3(0.0f, 0.3f, -3.0f);
		m_Acceleration = glm::vec3(0.0f);
		m_AccelerationMagnitude = glm::vec3(0.0f);

		m_Model.Load("assets/models/trex/trex.gltf");
		m_EnvironmentModel.Load("assets/models/env/scene.gltf");

		MT_LOG_INFO("\n	Creating trex texture:");
		m_Texture = Image::Create("assets/textures/trex.jpeg");

		MT_LOG_INFO("\n	Creating environment texture:");
		m_EnvTexture = Image::Create("assets/textures/env.png");
		m_Sampler = Sampler::Create(
			{
				SamplerFilter::LINEAR,
				SamplerFilter::LINEAR,
				MipmapMode::LINEAR,
				SamplerAddressMode::REPEAT,
				16.0f
			}
		);
		m_DescriptorSet = DescriptorSet::Create({
			{
				0,
				ResourceType::IMAGE,
				ShaderStage::FRAGMENT,
				1
			}
		});
		m_EnvDescriptorSet = DescriptorSet::Create({
			{
				0,
				ResourceType::IMAGE,
				ShaderStage::FRAGMENT,
				1
			}
		});
		m_DescriptorSet->UpdateDescriptor(0, m_Texture, m_Sampler);
		m_EnvDescriptorSet->UpdateDescriptor(0, m_EnvTexture, m_Sampler);

		MT_LOG_TRACE("Test layer attached");
	};

	void OnDetach() override
	{
		MT_LOG_INFO("~Test layer on detach~");
		m_Pipeline.reset();
		m_Texture.reset();
		m_EnvTexture.reset();
		m_Model.Release();
		m_EnvironmentModel.Release();
		m_DescriptorSet.reset();
		m_EnvDescriptorSet.reset();
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
	Ref<Shader> m_Shader;
	Model m_Model;
	Model m_EnvironmentModel;
	Ref<DescriptorSet> m_DescriptorSet;
	Ref<DescriptorSet> m_EnvDescriptorSet;
	Ref<Image> m_Texture;
	Ref<Image> m_EnvTexture;
	Ref<Sampler> m_Sampler;
	
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
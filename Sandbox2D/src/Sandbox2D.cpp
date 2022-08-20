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

		m_QuadPosition.x += m_Acceleration.x * m_AccelerationMagnitude.x;
		m_QuadPosition.y += m_Acceleration.y * m_AccelerationMagnitude.y;
		m_AccelerationMagnitude -= 0.001f;
		if (m_AccelerationMagnitude.x < 0.0f) m_AccelerationMagnitude.x = 0.0f;
		if (m_AccelerationMagnitude.y < 0.0f) m_AccelerationMagnitude.y = 0.0f;

		Renderer::BeginScene({ RefAs<Camera>(m_Camera) });
		Renderer::Submit(m_VBO, m_IBO, m_Pipeline, { m_MeshDescriptorSet }, glm::vec3(0.0f, 0.0f, -15.0f));
		Renderer::Submit(m_VBO, m_IBO, m_Pipeline, { m_MeshDescriptorSet }, m_QuadPosition);

		Renderer::EndScene();
	};

	void OnAttach() override
	{
		m_Shader = Shader::Create("basic.glsl");
		VertexBufferLayout bufferLayout({
			{ "aPos", ShaderDataType::FLOAT2 },
			{ "aColor", ShaderDataType::FLOAT3 },
			{ "aTexCoord", ShaderDataType::FLOAT2 },
		});

		m_Pipeline = Pipeline::Create({
			m_Shader,
			bufferLayout,
			PolygonMode::FILL
		});

		float vertices[] = {
			// Position		// Color			// Texture Coords
			-0.5f, -0.5f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f,	
			 0.5f, -0.5f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
			 0.5f,  0.5f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
			-0.5f,  0.5f,	1.0f, 1.0f, 1.0f,	0.0f, 0.0f
		};

		uint8_t indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_VBO = VertexBuffer::Create(vertices, sizeof(vertices), 0);
		m_IBO = IndexBuffer::Create(indices, sizeof(indices), 0, IndexType::UINT8);
		m_Texture = Image::Create("assets/textures/blending_test.png");
		m_DefaultSampler = Sampler::Create(
			{
				SamplerFilter::LINEAR,
				SamplerFilter::NEAREST,
				MipmapMode::LINEAR,
				SamplerAddressMode::REPEAT,
				16.0f
			}
		);
		m_MeshDescriptorSet = DescriptorSet::Create({
			{
				0,
				ResourceType::IMAGE,
				ShaderStage::FRAGMENT,
				1
			}
		});

		m_MeshDescriptorSet->UpdateDescriptor(0, m_Texture, m_DefaultSampler);

		m_Camera = CreateRef<Camera3D>();
		m_Camera->SetProjection(80.0f, 1.6f / 0.9f, 0.1f, 100.0f);
		m_Camera->SetSensivity(0.1f);

		// Physics
		m_QuadPosition = glm::vec3(0.0f, 0.0f, -10.0f);
		m_Acceleration = glm::vec3(0.0f);
		m_AccelerationMagnitude = glm::vec3(0.0f);


		MT_LOG_TRACE("Test layer attached");
	};

	void OnDetach() override
	{
		m_Pipeline.reset();
		m_VBO.reset();
		m_IBO.reset();
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
	Ref<VertexBuffer> m_VBO;
	Ref<IndexBuffer> m_IBO;
	Ref<Image> m_Texture;
	Ref<Sampler> m_DefaultSampler;
	Ref<Pipeline> m_Pipeline;
	Ref<Shader> m_Shader;
	Ref<DescriptorSet> m_MeshDescriptorSet;

	// Dynamic data
	Ref<Camera3D> m_Camera;

	// Physics
	glm::vec3 m_QuadPosition;
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
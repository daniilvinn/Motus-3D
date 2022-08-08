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
		if (Input::KeyPressed(KeyCode::KEY_W)) {
			//m_QuadPosition.y += 0.001f;
			m_Acceleration.y = 0.0f;
			m_Acceleration.y += 0.001f;
			m_AccelerationMagnitude.y = 1.0f;
		};
		if (Input::KeyPressed(KeyCode::KEY_A)) {
			//m_QuadPosition.x -= 0.001f;
			m_Acceleration.x = 0.0f;
			m_Acceleration.x -= 0.001f;
			m_AccelerationMagnitude.x = 1.0f;
		};
		if (Input::KeyPressed(KeyCode::KEY_S)) {
			//m_QuadPosition.y -= 0.001f;
			m_Acceleration.y = 0.0f;
			m_Acceleration.y -= 0.001f;
			m_AccelerationMagnitude.y = 1.0f;
		};
		if (Input::KeyPressed(KeyCode::KEY_D)) {
			//m_QuadPosition.x += 0.001f;
			m_Acceleration.x = 0.0f;
			m_Acceleration.x += 0.001f;
			m_AccelerationMagnitude.x = 1.0f;
		};

		m_QuadPosition.x += m_Acceleration.x * m_AccelerationMagnitude.x;
		m_QuadPosition.y += m_Acceleration.y * m_AccelerationMagnitude.y;
		m_AccelerationMagnitude -= 0.001f;
		if (m_AccelerationMagnitude.x < 0.0f) m_AccelerationMagnitude.x = 0.0f;
		if (m_AccelerationMagnitude.y < 0.0f) m_AccelerationMagnitude.y = 0.0f;

		Renderer::BeginScene({ m_VPmatrix });
		Renderer::Submit(m_VBO, m_IBO, m_Pipeline, m_QuadPosition);
		Renderer::EndScene();
	};

	void OnAttach() override
	{
		m_Shader = Shader::Create("basic.glsl");
		VertexBufferLayout bufferLayout({
			{"a_Pos", ShaderDataType::FLOAT2},
			{"a_Color", ShaderDataType::FLOAT3},
		});

		m_Pipeline = Pipeline::Create({
			m_Shader,
			bufferLayout,
			PolygonMode::FILL
		});

		float vertices[] = {
			-0.5f, -0.5f,	1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,	0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,	0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f,	1.0f, 1.0f, 1.0f
		};

		uint8_t indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_VBO = VertexBuffer::Create(vertices, sizeof(vertices), 0);
		m_IBO = IndexBuffer::Create(indices, sizeof(indices), 0, IndexType::UINT8);
		
		m_VPmatrix = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

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
	};

private:

	bool OnWindowResize(WindowResizedEvent& e) 
	{
		m_VPmatrix = glm::perspective(45.0f, (float)e.width / (float)e.height, 0.1f, 100.0f);
		return false;
	}

private:

	// Static data
	Ref<VertexBuffer> m_VBO;
	Ref<IndexBuffer> m_IBO;
	Ref<Pipeline> m_Pipeline;
	Ref<Shader> m_Shader;

	// Dynamic data
	glm::mat4 m_VPmatrix;

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
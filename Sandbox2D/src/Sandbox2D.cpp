#include <Motus3D.h>
using namespace Motus3D;

class TestLayer : public Motus3D::Layer
{
public:
	void OnUpdate() override
	{
		Renderer::BeginRender();
		Renderer::Submit(m_VBO, m_IBO, m_Pipeline);
		Renderer::EndRender();

	};

	void OnEvent(Event& e) override
	{
		
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

		MT_LOG_TRACE("Test layer attached");
	};

	void OnDetach() override
	{
		m_Pipeline.reset();
		m_VBO.reset();
		m_IBO.reset();
		MT_LOG_TRACE("Test layer detached");
	};

private:

	Ref<VertexBuffer> m_VBO;
	Ref<IndexBuffer> m_IBO;
	Ref<Pipeline> m_Pipeline;
	Ref<Shader> m_Shader;

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
#include <Motus3D.h>

class TestLayer : public Motus3D::Layer
{
public:
	void OnUpdate() override
	{
		
	};

	void OnEvent(Event& e) override
	{
		
	};

	void OnAttach() override
	{
		MT_LOG_TRACE("Test layer attached");
	};

	void OnDetach() override
	{
		MT_LOG_TRACE("Test layer detached");
	};

private:

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
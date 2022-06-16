#include <Motus3D.h>

class Sandbox2D : public Motus3D::Application
{
public:
	Sandbox2D(){};
	~Sandbox2D() override {};
};

Motus3D::Scope<Motus3D::Application> CreateApplication()
{
	return Motus3D::CreateScope<Sandbox2D>();
}
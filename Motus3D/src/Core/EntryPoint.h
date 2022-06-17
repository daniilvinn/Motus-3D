#pragma once

#ifdef MT_PLATFORM_WINDOWS
extern Motus3D::Scope<Motus3D::Application> CreateApplication();

// Entry Point
int main() {

	Motus3D::Scope<Motus3D::Application> Application = CreateApplication();
	Application->Run();
	Application.reset();

}
#else
	#error MOTUS: Motus currently supports only Windows platform!
#endif
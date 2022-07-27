#pragma once

extern Motus3D::Scope<Motus3D::Application> CreateApplication();

#ifdef MT_PLATFORM_WINDOWS
	#ifdef MT_DIST
	// Entry Point
	INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		PSTR lpCmdLine, INT nCmdShow)
	{
		Motus3D::Scope<Motus3D::Application> Application = CreateApplication();
		Application->Run();
		Application.reset();
	}
	#else
	int main(int argc, char** argv)
	{
		Motus3D::Scope<Motus3D::Application> Application = CreateApplication();
		Application->Run();
		Application.reset();
	}
	#endif
#else
	#error MOTUS: Motus currently supports only Windows platform!
#endif
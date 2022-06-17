#include "Application.h"
#include <iostream>
#include <Core/Logger.h>

namespace Motus3D
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		CoreLogger::Init();
		ClientLogger::Init();

		if(s_Instance)
		{
			MT_CORE_LOG_FATAL("Application already exists!");
			return;
		}

		s_Instance = this;

	}
	
	Application::~Application()
	{
		
	}

	void Application::Run()
	{

	}

}

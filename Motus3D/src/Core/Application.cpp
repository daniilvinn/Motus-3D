#include "Application.h"
#include <iostream>


namespace Motus3D
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		if(s_Instance)
		{
			std::cerr << "Application already exists!" << std::endl;
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

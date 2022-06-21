#include "Application.h"
#include <iostream>
#include <Core/Logger.h>
#include <Platform/Windows/WinApplicationWindow.h>
#include <Core/Events/Event.h>

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

		m_AppWindow = CreateRef<WinApplicationWindow>();
		m_AppWindow->SetEventHandle(MT_BIND_EVENT_FUNCTION(Application::OnEvent));

	}
	
	Application::~Application()
	{
		
	}

	void Application::Run()
	{

		while (m_ApplicationRunning)
		{
			ProcessEvents();

			for(auto& layer : m_LayerStack)
			{
				layer->OnUpdate();
			}

			m_AppWindow->OnUpdate();
		}
	}

	void Application::OnEvent(Event e)
	{
		// TODO
		m_EventQueue.Push(e);
		if(m_EventQueue.GetActualAmount() == APP_CONFIG_MAX_EVENT_QUEUE_SIZE)
		{
			ProcessEvents();
		}
	}

	bool Application::OnWindowResize(WindowResizedEvent& )
	{
		MT_CORE_LOG_TRACE("OnWindowResize");
		return true;
	}

	bool Application::OnWindowClosed(WindowClosedEvent& )
	{
		m_ApplicationRunning = false;
		return false;
	}

	void Application::ProcessEvents()
	{
		// TODO
		for(int i = 0; i < m_EventQueue.GetActualAmount(); i++){

			Event e = m_EventQueue[i];

			EventDispatcher dispatcher(e);

			dispatcher.Dispatch<WindowClosedEvent>(MT_BIND_EVENT_FUNCTION(Application::OnWindowClosed));
			dispatcher.Dispatch<WindowResizedEvent>(MT_BIND_EVENT_FUNCTION(Application::OnWindowResize));

			MT_CORE_LOG_TRACE(e.ToString());

			for(auto& layer : m_LayerStack)
			{
				if (e.IsHandled()) break;
				layer->OnEvent(e);
			}
		}
		m_EventQueue.Clear();
	}
}

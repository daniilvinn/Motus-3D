#include "Application.h"
#include <iostream>
#include <Core/Logger.h>
#include <Platform/Windows/WinApplicationWindow.h>
#include <Core/Events/ApplicationEvent.h>

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

	void Application::OnEvent(Event* e)
	{
		m_EventQueue.push(e);
	}

	bool Application::OnWindowResize(WindowResizeEvent& )
	{
		MT_CORE_LOG_TRACE("OnWindowResize");
		return true;
	}

	bool Application::OnWindowClosed(WindowCloseEvent& )
	{
		m_ApplicationRunning = false;
		return false;
	}

	void Application::ProcessEvents()
	{
		while(!m_EventQueue.empty())
		{
			Event* e = m_EventQueue.front();

			EventDispatcher dispatcher(*e);

			dispatcher.Dispatch<WindowCloseEvent>(MT_BIND_EVENT_FUNCTION(Application::OnWindowClosed));
			dispatcher.Dispatch<WindowResizeEvent>(MT_BIND_EVENT_FUNCTION(Application::OnWindowResize));

			MT_CORE_LOG_TRACE(e->GetEventName());

			for(auto& layer : m_LayerStack)
			{
				if (e->m_IsHandled) break;
				layer->OnEvent(*e);
			}

			delete e;
			m_EventQueue.pop();
		}
	}
}

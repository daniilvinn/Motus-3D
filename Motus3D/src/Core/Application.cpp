#include "Application.h"
#include <Core/Logger.h>
#include <Platform/Windows/WinApplicationWindow.h>
#include <Core/Events/Event.h>

#include "Visus.h"

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
		for (auto& layer : m_LayerStack) {
			layer->OnDetach();
			delete layer;
		}

		Renderer::Shutdown();
	}

	void Application::Run()
	{
		while (m_ApplicationRunning)
		{
			ProcessEvents();
			m_AppWindow->OnUpdate();

			Renderer::BeginFrame();
			Renderer::ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			for(auto& layer : m_LayerStack)
			{
				layer->OnUpdate();
			}
			Renderer::EndFrame();
		}
	}

	void Application::OnEvent(Event e)
	{
		m_EventQueue.Push(e);
		if(m_EventQueue.GetActualAmount() == APP_CONFIG_MAX_EVENT_QUEUE_SIZE)
		{
			ProcessEvents();
		}
	}

	bool Application::OnWindowResize(WindowResizedEvent& e)
	{
		// TODO: vvv Causes crash, to be fixed vvv
		// Motus3D::Renderer::OnWindowResize(e.width, e.height);
		return false;
	}

	bool Application::OnWindowClosed(WindowClosedEvent& e)
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

			for(auto& layer : m_LayerStack)
			{
				if (e.IsHandled()) break;
				layer->OnEvent(e);
			}
		}
		m_EventQueue.Clear();
	}
}

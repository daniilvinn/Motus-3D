#pragma once

#include <Core/Macros.h>
#include <Core/ApplicationWindow.h>
#include <Core/LayerStack.h>
#include <array>

#include <Core/Events/Event.h>
#include <Core/Events/EventQueue.h>

namespace Motus3D
{
	class MOTUS_API Application
	{
	public:
		Application();
		virtual ~Application();

		Application* Get() { return s_Instance; }
		void Run();

		void AddLayer(Layer* layer) { m_LayerStack.AddLayer(layer); };
		void OnEvent(Event e);

	private:

		bool OnWindowResize(WindowResizedEvent& e);
		bool OnWindowClosed(WindowClosedEvent& e);
		void ProcessEvents();

		static Application* s_Instance;
		Ref<ApplicationWindow> m_AppWindow;
		LayerStack m_LayerStack;

		EventQueue m_EventQueue;

		bool m_ApplicationRunning;

	};
}
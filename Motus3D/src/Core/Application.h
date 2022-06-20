#pragma once

#include <Core/Macros.h>
#include <Core/ApplicationWindow.h>
#include <Core/LayerStack.h>
#include <queue>

#include <Core/Events/ApplicationEvent.h>

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
		void OnEvent(Event* e);

	private:

		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClosed(WindowCloseEvent& e);
		void ProcessEvents();

		static Application* s_Instance;
		Ref<ApplicationWindow> m_AppWindow;
		LayerStack m_LayerStack;

		std::queue<Event*> m_EventQueue;

		bool m_ApplicationRunning;

	};
}
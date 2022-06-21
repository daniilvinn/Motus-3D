#include "WinApplicationWindow.h"

#include <Core/Macros.h>
#include <Core/Logger.h>

#include <Core/Events/Event.h>


// HACK: setting GLFW error callback here isn't good idea. To be moved to another place.

namespace Motus3D
{
	static void glfwErrorCallback(int code, const char* error)
	{
		MT_CORE_LOG_FATAL("GLFW_ERROR: {0} ({1})", error, code);
	}

	WinApplicationWindow::WinApplicationWindow(uint32_t width, uint32_t height, std::string title)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Handle = glfwCreateWindow(width, height, title.c_str(), nullptr , nullptr);

		m_WindowData.width = width;
		m_WindowData.height = height;
		m_WindowData.title = title;

		glfwSetWindowUserPointer(m_Handle, &m_WindowData);

		glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.width = width;
			data.height = height;

			WindowResizedEvent variant;
			variant.width = width;
			variant.height = height;
			Event event(variant, WindowResized);

			data.eventHandler(event);
		});
		glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowClosedEvent variant;
			Event event(variant, WindowClosed);
			data.eventHandler(event);
		});
		glfwSetKeyCallback(m_Handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
				case GLFW_PRESS: {
					KeyPressedEvent variant;
					variant.keycode = key;
					variant.repeatcount = 0;
					Event e(variant, KeyPressed);
					data.eventHandler(e);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent variant;
					variant.keycode = key;
					variant.repeatcount = 1;
					Event e(variant, KeyPressed);
					data.eventHandler(e);
					data.eventHandler(e);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent variant;
					variant.keycode = key;
					Event e(variant, KeyReleased);
					data.eventHandler(e);
					break;
				}
				default: {
					MT_CORE_ASSERT(false, "Invalid keyboard event action type");
					break;
				}
			}
		});
		glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* window, double xpos, double ypos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent variant;
			variant.xpos = xpos;
			variant.ypos = ypos;

			Event event(variant, MouseMoved);

			data.eventHandler(event);
		});
		glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent variant;
					variant.button = button;
					Event event{ variant, MouseButtonPressed };
					data.eventHandler(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent variant;
					variant.button = button;
					Event event{ variant, MouseButtonReleased };
					data.eventHandler(event);
					break;
				}

				default: {
					MT_CORE_ASSERT(false, "Invalid mouse event action type");
					break;
				}
			}
		});
		glfwSetScrollCallback(m_Handle, [](GLFWwindow* window, double xoffset, double yoffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent variant;
			variant.xoffset = xoffset;
			variant.yoffset = yoffset;
			Event event(variant, MouseScrolled);
			data.eventHandler(event);
			});			// m_Handle 
		glfwSetCharCallback(m_Handle, [](GLFWwindow* window, unsigned int character) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent variant;
			variant.keycode = character;
			Event event(variant, KeyTyped);
			data.eventHandler(event);
			});
		glfwSetErrorCallback(glfwErrorCallback);
	}
	
	WinApplicationWindow::~WinApplicationWindow()
	{
		glfwDestroyWindow(m_Handle);
		glfwTerminate();
	}

	void WinApplicationWindow::OnUpdate()
	{
		glfwPollEvents();
	}


}

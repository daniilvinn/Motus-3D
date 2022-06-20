#include "WinApplicationWindow.h"

#include <Core/Macros.h>
#include <Core/Logger.h>

#include <Core/Events/ApplicationEvent.h>
#include <Core/Events/MouseEvent.h>
#include <Core/Events/KeyEvent.h>


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

			WindowResizeEvent* event = new WindowResizeEvent(width, height);

			data.eventHandler(event);
		});
		glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent* event = new WindowCloseEvent;
			data.eventHandler(event);
		});
		glfwSetKeyCallback(m_Handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
				case GLFW_PRESS: {
					KeyPressedEvent* event = new KeyPressedEvent(key, 0);
					data.eventHandler(event);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent* event = new KeyPressedEvent(key, 1);
					data.eventHandler(event);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent* event = new KeyReleasedEvent(key);
					data.eventHandler(event);
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

			MouseMovedEvent* event = new MouseMovedEvent((uint16_t)xpos, (uint16_t)ypos);
			data.eventHandler(event);
		});
		glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent* event = new MouseButtonPressedEvent(button);
					data.eventHandler(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent* event = new MouseButtonReleasedEvent(button);
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
			MouseScrolledEvent* event = new MouseScrolledEvent(xoffset, yoffset);
			data.eventHandler(event);
			});			// m_Handle 
		glfwSetCharCallback(m_Handle, [](GLFWwindow* window, unsigned int character) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent* event = new KeyTypedEvent(character);
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

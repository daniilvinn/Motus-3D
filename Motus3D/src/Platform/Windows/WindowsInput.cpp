#include "WindowsInput.h"

#include <GLFW/glfw3.h>
#include <Core/Application.h>

namespace Motus3D {

	bool WindowsInput::KeyPressed_Impl(KeyCode code)
	{
		int status = glfwGetKey((GLFWwindow*)Application::Get()->GetWindow()->GetHandle(), static_cast<int>(code));
		return status == GLFW_PRESS || status == GLFW_REPEAT ? true : false ;
	}

	bool WindowsInput::MouseButtonPressed_Impl(KeyCode code)
	{
		int status = glfwGetMouseButton((GLFWwindow*)Application::Get()->GetWindow()->GetHandle(), static_cast<int>(code));
		return status == GLFW_PRESS ? true : false;
	}

	std::pair<int32_t, int32_t> WindowsInput::MousePosition_Impl()
	{
		double xpos, ypos;
		glfwGetCursorPos((GLFWwindow*)Application::Get()->GetWindow()->GetHandle(), &xpos, &ypos);
		return { xpos, ypos };
	}

	float WindowsInput::MouseScrolledY_Impl()
	{
		MT_CORE_ASSERT(false, "Method not implemented");
		assert(true);
		return 0.0f;
	}

	float WindowsInput::MouseScrolledX_Impl()
	{
		MT_CORE_ASSERT(false, "Method not implemented");
		return 0.0f;
	}

	void WindowsInput::LockMouse_Impl()
	{
		GLFWwindow* window = (GLFWwindow*)Application::Get()->GetWindow()->GetHandle();
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void WindowsInput::ReleaseMouse_Impl()
	{
		GLFWwindow* window = (GLFWwindow*)Application::Get()->GetWindow()->GetHandle();
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}
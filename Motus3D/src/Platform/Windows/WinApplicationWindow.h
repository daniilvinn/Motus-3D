#pragma once

#include <Core/ApplicationWindow.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Motus3D
{
	class WinApplicationWindow final : public ApplicationWindow
	{
	public:
		WinApplicationWindow(uint32_t width = 1600, uint32_t height = 900, std::string title = "Motus3D");
		~WinApplicationWindow() override;

		void SetEventHandle(const EventHandlerFn& fn) override { m_WindowData.eventHandler = fn; }
		void OnUpdate() override;

		// Returns void pointer, therefore to be casted to GLFWwindow*
		void* GetHandle() override { return m_Handle; };
		std::pair<uint32_t, uint32_t> GetExtent() override { return { 0, 0 }; }


	private:
		GLFWwindow* m_Handle;

		struct WindowData
		{
			std::string title;
			uint32_t width;
			uint32_t height;
			EventHandlerFn eventHandler;
		} m_WindowData;

	};

}
#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/Swapchain.h>

#include <GLFW/glfw3.h>

namespace Visus
{
	struct VISUS_API ContextSpecification
	{
		GLFWwindow* windowHandle;
	};

	class VISUS_API GraphicsContext
	{
	public:
		GraphicsContext() = default;
		~GraphicsContext() = default;

		static Ref<GraphicsContext> Create();

		virtual void Init(const ContextSpecification& spec) = 0;
		virtual void Shutdown() = 0;

		//virtual Ref<Swapchain> GetSwapchain() = 0;

	};
}


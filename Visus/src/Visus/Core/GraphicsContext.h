#pragma once

#include <Visus/Core/Macros.h>
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
		static Ref<GraphicsContext> Get() { return s_Instance; }

		virtual void Init(const ContextSpecification& spec) = 0;
		virtual void Shutdown() = 0;

	protected:


	private:
		static Ref<GraphicsContext> s_Instance;

	};
}


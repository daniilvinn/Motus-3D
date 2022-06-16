#pragma once

#include <Core/Macros.h>

namespace Motus3D
{
	class MOTUS_API Application
	{
	public:
		Application();
		virtual ~Application();

		Application* Get() { return s_Instance; }
		void Run();

	private:
		static Application* s_Instance;
	};
}


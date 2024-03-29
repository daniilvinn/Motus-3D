#pragma once

#include <Visus/Core/Macros.h>

namespace Motus3D
{
	// Abstract from GLFW
	struct VISUS_API ContextSpecification
	{
		void* windowHandle;
		uint8_t framesInFlight;
	};

	class VISUS_API GraphicsContext
	{
	public:
		GraphicsContext() = default;
		~GraphicsContext() = default;

		static Ref<GraphicsContext> Create();

		virtual void Init(const ContextSpecification& spec) = 0;
		virtual void Shutdown() = 0;

	};
}


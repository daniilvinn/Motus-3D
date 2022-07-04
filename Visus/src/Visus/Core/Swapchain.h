#pragma once

#include <Visus/Core/Macros.h>

namespace Visus {
	class VISUS_API Swapchain
	{
	public:
		~Swapchain(){}

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

	};
}
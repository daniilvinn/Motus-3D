#pragma once

#include <Visus/Core/Macros.h>

namespace Motus3D {
	class VISUS_API Swapchain
	{
	public:
		~Swapchain(){}

		virtual void Destroy() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual uint8_t GetCurrentFrameIndex() = 0;
		virtual std::pair<uint32_t, uint32_t> GetExtent() const = 0;

	};
}
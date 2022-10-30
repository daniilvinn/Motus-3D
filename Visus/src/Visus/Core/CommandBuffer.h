#pragma once

#include <Visus/Core/Macros.h>

namespace Motus3D {

	enum class VISUS_API CommandBufferType : uint8_t {
		GRAPHICS_TRANSFER, // used to record both graphics and transfer commands
		COMPUTE, // used to record compute commands (e.g. vkDispatch*())
		NONE // non-allocated command buffer with no type.
	};

	enum class VISUS_API CommandBufferCreateFlags : uint32_t {
		TRANSIENT = 0x0001 // Create short-living command buffer that will be freed soon.
	};

	// Interface for comm
	class VISUS_API CommandBuffer {
	public:
		static Ref<CommandBuffer> Create(CommandBufferType type, uint32_t flags);

		virtual CommandBufferType GetType() = 0;
		virtual bool IsTransient() = 0;
		
		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void Allocate(CommandBufferType type, uint32_t flags) = 0;
		virtual void Reset() = 0;
		virtual void Release() = 0;


	};

}
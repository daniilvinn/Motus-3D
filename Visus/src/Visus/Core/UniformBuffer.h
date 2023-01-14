#pragma once

#include <Visus/Core/Macros.h>

namespace Motus3D {

	class VISUS_API UniformBuffer
	{
	public:
		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
		static Ref<UniformBuffer> Create(void* data, uint32_t size, uint32_t binding);

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual size_t GetSize() const = 0;

	protected:
		UniformBuffer() {};
	};

}
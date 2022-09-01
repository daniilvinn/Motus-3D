#pragma once

#include <Visus/Core/Macros.h>
namespace Motus3D {

	class VISUS_API VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {};

		static Ref<VertexBuffer> Create(uint64_t size);
		static Ref<VertexBuffer> Create(void* data, uint64_t size, uint64_t offset);

		virtual void Release() = 0;

	protected:

		virtual void SetData(void* data, uint64_t size, uint64_t offset) = 0;

	};

}
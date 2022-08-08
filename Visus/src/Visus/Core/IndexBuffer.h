#pragma once

#include <Visus/Core/Macros.h>

namespace Motus3D {


	enum class VISUS_API IndexType : int
	{
		UINT8 = 0,
		UINT16 = 1,
		UINT32 = 2
	};

	class VISUS_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {};

		static Ref<IndexBuffer> Create(uint64_t size);
		static Ref<IndexBuffer> Create(void* data, uint64_t size, uint64_t offset, IndexType type);

		virtual uint32_t GetCount() const = 0;

	};

}
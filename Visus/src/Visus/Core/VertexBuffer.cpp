#include "VertexBuffer.h"

#include <Visus/Platform/VulkanVertexBuffer.h>

namespace Motus3D {
	Ref<VertexBuffer> VertexBuffer::Create(uint64_t size)
	{
		return CreateRef<VulkanVertexBuffer>(size);
	}

	Ref<VertexBuffer> VertexBuffer::Create(void* data, uint64_t size, uint64_t offset)
	{
		return CreateRef<VulkanVertexBuffer>(data, size, offset);
	}
}
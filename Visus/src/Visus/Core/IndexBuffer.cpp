#include "IndexBuffer.h"

#include <Visus/Platform/VulkanIndexBuffer.h>

namespace Motus3D {

	Ref<IndexBuffer> IndexBuffer::Create(uint64_t size)
	{
		return CreateRef<VulkanIndexBuffer>(size);
	}

	Ref<IndexBuffer> IndexBuffer::Create(void* data, uint64_t size, uint64_t offset, IndexType type)
	{
		return CreateRef<VulkanIndexBuffer>(data, size, offset, type);
	}
}
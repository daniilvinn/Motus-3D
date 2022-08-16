#include "UniformBuffer.h"

#include <Visus/Platform/VulkanUniformBuffer.h>

namespace Motus3D 
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		return CreateRef<VulkanUniformBuffer>(size, binding);
	}

	Ref<UniformBuffer> UniformBuffer::Create(void* data, uint32_t size, uint32_t binding)
	{
		return CreateRef<VulkanUniformBuffer>(data, size, binding);
	}
}
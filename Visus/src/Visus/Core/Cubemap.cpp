#include <Visus/Core/Cubemap.h>

#include <Visus/Platform/VulkanCubemap.h>

namespace Motus3D {

	Ref<Cubemap> Cubemap::Create(std::string directory)
	{
		return CreateRef<VulkanCubemap>(directory);
	}

}
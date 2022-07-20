#include <Visus/Core/GraphicsContext.h>
#include <Visus/Platform/VulkanGraphicsContext.h>

namespace Motus3D
{

	Ref<GraphicsContext> GraphicsContext::Create()
	{
		return CreateRef<VulkanGraphicsContext>();
	}
}

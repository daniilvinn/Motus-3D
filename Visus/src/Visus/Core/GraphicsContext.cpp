#include <Visus/Core/GraphicsContext.h>
#include <Visus/Platform/VulkanGraphicsContext.h>

namespace Visus
{

	Ref<GraphicsContext> GraphicsContext::Create()
	{
		return CreateRef<VulkanGraphicsContext>();
	}
}

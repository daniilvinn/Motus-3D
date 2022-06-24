#include <Visus/Core/GraphicsContext.h>
#include <Visus/Platform/VulkanGraphicsContext.h>

namespace Visus
{
	Ref<GraphicsContext> GraphicsContext::s_Instance = CreateRef<VulkanGraphicsContext>();
}

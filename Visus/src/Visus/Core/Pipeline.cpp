#include "Pipeline.h"

#include <Visus/Platform/VulkanPipeline.h>

namespace Motus3D
{

	Ref<Pipeline> Pipeline::Create(PipelineSpecification specification)
	{
		return CreateRef<VulkanPipeline>(specification);
	}


}


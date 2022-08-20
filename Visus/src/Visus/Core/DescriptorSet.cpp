#include "DescriptorSet.h"

#include <Visus/Platform/VulkanDescriptorSet.h>

namespace Motus3D {

	Ref<DescriptorSet> DescriptorSet::Create(std::initializer_list<Descriptor> layout)
	{
		return CreateRef<VulkanDescriptorSet>(layout);
	}

}
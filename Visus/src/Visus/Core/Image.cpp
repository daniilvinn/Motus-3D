#include "Image.h"

#include <Visus/Platform/VulkanImage.h>

namespace Motus3D {
	
	// Image
	Ref<Image> Image::Create(ImageUsage usage, std::string filepath)
	{
		return CreateRef<VulkanImage>(usage, filepath);
	}

	// Sampler
	Ref<Sampler> Sampler::Create(SamplerSpecification specification) {
		return CreateRef<VulkanSampler>(specification);
	}

}
#include "Image.h"

#include <Visus/Platform/VulkanImage.h>

namespace Motus3D {
	
	// Image
	Ref<Image> Image::Create(std::string filepath, ImageFormat format)
	{
		return CreateRef<VulkanImage>(filepath, format);
	}

	// Sampler
	Ref<Sampler> Sampler::Create(SamplerSpecification specification) {
		return CreateRef<VulkanSampler>(specification);
	}

}
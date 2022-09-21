#pragma once

#include <Visus/Core/Macros.h>

namespace Motus3D {

	// Image
	class VISUS_API Image
	{
	public:
		static Ref<Image> Create(std::string filepath);
		virtual std::string_view GetFilepath() const = 0;

	};

	// Sampler and data which corresponds to it
	enum class VISUS_API SamplerFilter : uint8_t {
		NEAREST,
		LINEAR
	};

	enum class VISUS_API MipmapMode : uint8_t 
	{
		NEAREST,
		LINEAR
	};

	enum class VISUS_API SamplerAddressMode : uint8_t {
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP,
		CLAMP_BORDER
	};

	struct VISUS_API SamplerSpecification 
	{
		SamplerFilter minFilter;
		SamplerFilter magFilter;
		MipmapMode mipmapMode;
		SamplerAddressMode addressMode;
		float anisotropy;
	};

	class VISUS_API Sampler 
	{
	public:
		static Ref<Sampler> Create(SamplerSpecification specification);
		virtual void Destroy() = 0;

	};

}
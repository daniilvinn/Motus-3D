#pragma once

#include <Visus/Core/Macros.h>

namespace Motus3D {

	// Deprecated
	// Specifies image channel width, e.g. 8, 16 or 32 bytes per channel
	// Prefix C - color
	// Prefix D - depth
	// Prefix S - stencil
	enum class VISUS_API ImageFormat : uint8_t {
		C8,		// 8-bit Color format
		C16,	// 16-bit Color format
		C32,	// 32-bit Color format
		D32,	// 32-bit Depth format without stencil attachment
		D24S8,	// 24-bit Depth format with 8-bit stencil attachment
		D32S8,	// 32-bit Depth format with 8-bit stencil attachment
		NONE
	};

	enum class VISUS_API ImageUsage {
		SHADER_READ_ONLY,
		RENDER_TARGET_HDR,
		RENDER_TARGET_LDR, // WARN: currently not supported, will cause engine crash.
	};

	// Image
	class VISUS_API Image
	{
	public:
		static Ref<Image> Create(ImageUsage usage, std::string filepath = "");
		virtual std::string_view GetFilepath() const = 0;
		
		/*
		* @brief ONLY to be used if image has usage "Render Target". Recreated image with new resolution.
		*/
		virtual void Invalidate() = 0;

		virtual void Release() = 0;

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
#pragma once

#include <Visus/Core/Macros.h>
#include <Visus/Core/UniformBuffer.h>
#include <Visus/Core/Image.h>

#include <Visus/Core/Shader.h>

namespace Motus3D {

	enum class VISUS_API ResourceType : uint8_t
	{
		UBO,
		SSBO,
		IMAGE
	};

	// This struct is used ONLY in DescriptorSet constructor. It is not standalone.
	struct VISUS_API Descriptor {
		uint8_t binding;
		ResourceType type;
		ShaderStage stage;
		uint8_t arrayElements; // 1 if it is not an array
	};

	class VISUS_API DescriptorSet
	{
	public:
		static Ref<DescriptorSet> Create(std::initializer_list<Descriptor> layout);

		// range's 0 value is special value, which specifies whole buffer to be bound to descriptor.
		virtual void UpdateDescriptor(uint8_t binding, uint32_t range, uint32_t offset, Ref<UniformBuffer> ubo, uint32_t arrayElement = 0) = 0;
		virtual void UpdateDescriptor(uint8_t binding, Ref<Image> image, Ref<Sampler> sampler, uint32_t arrayElement = 0) = 0;
		// virtual void UpdateDescriptor(uint8_t binding, Ref<StorageBuffer> ssbo) = 0;
		// virtual void UpdateDescriptor(uint8_t binding, Ref<Image3D> image) = 0;

		

	};

}
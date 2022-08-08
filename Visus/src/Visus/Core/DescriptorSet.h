#pragma once

#include <Visus/Core/Macros.h>

namespace Motus3D {

	enum class VISUS_API DescriptorType : uint8_t {
		UBO,
		SSBO,
		IMAGE
	};

	struct VISUS_API DescriptorSetElement {
		DescriptorType type;
		uint8_t binding;
		uint8_t count;
	};

	class VISUS_API DescriptorSet
	{
	public:
		static Ref<DescriptorSet> Create(std::initializer_list<DescriptorSetElement> layout);

	protected:
		DescriptorSet() {};
	private:
	};
}
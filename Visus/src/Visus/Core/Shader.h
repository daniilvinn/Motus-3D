#pragma once

#include <Visus/Core/Macros.h>

namespace Motus3D {

	enum class VISUS_API ShaderDataType : uint8_t
	{
		INT,
		INT2,
		INT3,
		INT4,
		FLOAT,
		FLOAT2,
		FLOAT3,
		FLOAT4,
		MAT3,
		MAT4,
		BOOL
	};

	class VISUS_API Shader
	{
	public:
		static Ref<Shader> Create(const std::string& filename);

		virtual ~Shader(){};
	};
}
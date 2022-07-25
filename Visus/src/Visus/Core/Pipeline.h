#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/Shader.h>
#include <Visus/Core/BufferLayout.h>

namespace Motus3D {

	enum class PolygonMode : uint8_t
	{
		FILL, // fill polygon
		LINES // fill only edges of polygon
	};

	struct PipelineSpecification
	{
		Ref<Shader> shader;
		VertexBufferLayout dataLayout;
		PolygonMode polygonMode;
	};

	class VISUS_API Pipeline
	{
	public:
		virtual ~Pipeline() {}

		static Ref<Pipeline> Create(PipelineSpecification specification);
		

	};
}
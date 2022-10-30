#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/Shader.h>
#include <Visus/Core/BufferLayout.h>

namespace Motus3D {

	enum class VISUS_API PipelineExecutionModel : uint8_t {
		GRAPHICS,
		COMPUTE
	};

	enum class VISUS_API PolygonMode : uint8_t
	{
		FILL, // fill polygon
		LINES, // fill only edges of polygon
		NONE // used for compute pipelines
	};

	enum class VISUS_API CullMode : uint8_t {
		NONE,
		BACK,
		FRONT
	};

	struct PipelineSpecification
	{
		Ref<Shader> shader;
		PipelineExecutionModel executionModel;
		std::string debugName;
		VertexBufferLayout* pDataLayout = nullptr; // nullptr if creating compute pipeline
		PolygonMode polygonMode = PolygonMode::NONE;
		CullMode cullMode = CullMode::NONE;
		bool depthTestEnabled = false;
	};

	class VISUS_API Pipeline
	{
	public:
		virtual ~Pipeline() {}

		static Ref<Pipeline> Create(PipelineSpecification specification);
		

	};
}
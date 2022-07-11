#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/Shader.h>

namespace Visus {

	struct PipelineSpecification
	{
		Ref<Shader> shader;
	};

	class VISUS_API Pipeline
	{
	public:
		virtual ~Pipeline() {};

		

	};
}
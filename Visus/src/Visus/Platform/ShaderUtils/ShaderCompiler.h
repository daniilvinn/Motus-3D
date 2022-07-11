#pragma once

#include <Visus/Core/Macros.h>

namespace Visus {

	enum ShaderStage
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
		COMPUTE = 2,
		MAX_VALUE = COMPUTE
	};

	// Visus currently supports only vertex, fragment and compute shaders.
	// Also visus supports only GLSL shaders.
	// TODO: compiling geometry shaders and HLSL compiling
	class ShaderCompiler
	{
	public:
		static std::vector<uint32_t> CompileToSpirv(const std::string& source, ShaderStage stage, std::string nametag);

	private:

	};
}
#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/Shader.h>

namespace Motus3D {

	// Motus3D currently supports only vertex, fragment and compute shaders.
	// Also Visus supports only GLSL shaders.
	// TODO: compiling geometry shaders and HLSL compiling
	class ShaderCompiler
	{
	public:
		static std::vector<uint32_t> CompileToSpirv(const std::string& source, ShaderStage stage, std::string nametag);

	private:

	};
}
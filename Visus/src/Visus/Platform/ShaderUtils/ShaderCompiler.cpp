#include "ShaderCompiler.h"

#include <Visus/Core/Logger.h>
#include <shaderc/shaderc.hpp>

namespace Visus
{
	constexpr shaderc_shader_kind VisusToShadercStage(ShaderStage stage)
	{
		switch (stage)
		{
		case VERTEX:
			return shaderc_glsl_vertex_shader;
		case FRAGMENT:
			return shaderc_glsl_fragment_shader;
		case COMPUTE:
			return shaderc_glsl_compute_shader;
		default:
			VISUS_ERROR("Shader Compiler: unsupported passed invalid shader stage!");
			break;
		}
	}

	std::vector<uint32_t> ShaderCompiler::CompileToSpirv(const std::string& source, ShaderStage stage, std::string nametag)
	{
		static shaderc::Compiler compiler; // No need to recreate it every time engine tries to compile shader
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
		options.SetSourceLanguage(shaderc_source_language_glsl);

		shaderc::SpvCompilationResult result =  compiler.CompileGlslToSpv(
			source.c_str(),
			source.length(),
			VisusToShadercStage(stage),
			nametag.c_str(),
			"main",
			options
		);

		if(result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			VISUS_ERROR("Failed to compiler shader ({0}): {1}", nametag, result.GetErrorMessage());
		} else
		{
			VISUS_INFO("Shader ({0}) compiled successfully!", nametag);
		}

		std::vector<uint32_t> binary_data(result.begin(), result.end());

		return binary_data;
	}
}

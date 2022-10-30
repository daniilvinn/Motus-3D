#include "ShaderCompiler.h"

#include <Visus/Core/Logger.h>
#include <shaderc/shaderc.hpp>

#include <fstream>
#include <filesystem>

namespace Motus3D
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

	std::string ShaderStageToFileExt(ShaderStage stage) {
		switch (stage)
		{
		case NONE:
			VISUS_ASSERT(false, "Attempted to convert shader with no type");
			break;
		case VERTEX:
			return std::string("vert");
			break;
		case FRAGMENT:
			return std::string("frag");
			break;
		case COMPUTE:
			return std::string("comp");
			break;
		}
	}

	std::vector<uint32_t> ShaderCompiler::CompileToSpirv(CompileData data, bool* result)
	{
		static shaderc::Compiler compiler; // No need to recreate it every time engine tries to compile shader
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
		options.SetSourceLanguage(shaderc_source_language_glsl);

		shaderc::SpvCompilationResult spv_result = compiler.CompileGlslToSpv(
			data.source.data(),
			data.source.length(),
			VisusToShadercStage(data.stage),
			data.name.c_str(),
			"main",
			options
		);

		if(spv_result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			VISUS_ERROR("Failed to compiler shader ({0}): {1}", data.name, spv_result.GetErrorMessage());
			*result = false;
		} else
		{
			VISUS_TRACE("Shader ({0}) compiled successfully!", data.name);
			*result = true;
		}

		std::vector<uint32_t> binary_data(spv_result.begin(), spv_result.end());

		std::string filepath = data.output_dir + std::string("bin_") + data.name + "." + ShaderStageToFileExt(data.stage);
		if (!std::filesystem::exists(data.output_dir)) {
			std::filesystem::create_directory(data.output_dir);
		};

		std::ofstream output_binary_stream(filepath, std::ios::out | std::ios::trunc | std::ios::binary);
		output_binary_stream.write((const char*)binary_data.data(), binary_data.size() * 4);
		output_binary_stream.close();

		return binary_data;
	}
}

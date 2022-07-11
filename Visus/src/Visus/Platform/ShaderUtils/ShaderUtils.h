#pragma once

#include <Visus/Core/Macros.h>
#include <Visus/Platform/ShaderUtils/ShaderCompiler.h>

namespace Visus
{
	class ShaderUtils
	{
	public:
		static Ref<ShaderCompiler> GetCompiler() { return s_Compiler; };
		static std::string GetAssetsDirectory() { return "assets/shaders/"; }

	private:
		static Ref<ShaderCompiler> s_Compiler;
	};
}

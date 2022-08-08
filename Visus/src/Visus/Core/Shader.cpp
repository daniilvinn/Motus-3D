#include "Shader.h"

#include <Visus/Platform/VulkanShader.h>

namespace Motus3D {

	Ref<Shader> Shader::Create(const std::string& filename)
	{
		return CreateRef<VulkanShader>(filename);
	}

}
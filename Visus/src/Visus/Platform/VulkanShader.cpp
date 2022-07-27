#include "VulkanShader.h"

#include <Visus/Platform/ShaderUtils/ShaderUtils.h>
#include <Visus/Platform/VulkanGraphicsContext.h>

#include <spirv_cross/spirv_glsl.hpp>
#include <shaderc/shaderc.hpp>
#include <fstream>

namespace Motus3D
{
	VulkanShader::VulkanShader()
	{
		
	}

	VulkanShader::VulkanShader(const std::string& filename)
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();

		auto compiler = ShaderUtils::GetCompiler();
		ParseFile(std::forward<const std::string&>(filename));

		if(m_ShaderSources[VERTEX].has_value())
		{
			std::vector<uint32_t> binary = compiler->CompileToSpirv(m_ShaderSources[VERTEX].value().second.str(), VERTEX, filename + " : vertex");
			m_BinaryData.emplace(VERTEX, binary);
		}
		if (m_ShaderSources[FRAGMENT].has_value())
		{
			std::vector<uint32_t> binary = compiler->CompileToSpirv(m_ShaderSources[FRAGMENT].value().second.str(), FRAGMENT, filename + " : fragment");
			m_BinaryData.emplace(FRAGMENT, binary);
		}
		if (m_ShaderSources[COMPUTE].has_value())
		{
			std::vector<uint32_t> binary = compiler->CompileToSpirv(m_ShaderSources[COMPUTE].value().second.str(), COMPUTE, filename + " : compute");
			m_BinaryData.emplace(COMPUTE, binary);
		}

		for (auto& shader : m_BinaryData)
		{
			VkShaderModule module;
			VkShaderModuleCreateInfo module_create_info = {};

			module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			module_create_info.pCode = shader.second.data();
			module_create_info.codeSize = shader.second.size() * 4;

			VK_CHECK_RESULT(vkCreateShaderModule(device->GetHandle(), &module_create_info, nullptr, &module));

			VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {};
			pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			pipeline_shader_stage_create_info.module = module;
			pipeline_shader_stage_create_info.stage = VisusToVulkanShaderStage(shader.first);
			pipeline_shader_stage_create_info.pName = "main";

			m_CreateInfos.push_back(pipeline_shader_stage_create_info);

		}

		for(auto& src : m_ShaderSources)
		{
			if(src.has_value())
			{
				src.value().second.clear();
			}
		}

	}

	VulkanShader::~VulkanShader()
	{
		VISUS_TRACE("Destroying shader...");
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();

		for(auto& src : m_ShaderSources)
		{
			if(src.has_value())
			{
				src.value().second.clear();
			}
		}

		for(auto& data : m_BinaryData)
		{
			data.second.clear();
		}
	}

	void VulkanShader::ParseFile(const std::string& filename)
	{
		std::ifstream file(ShaderUtils::GetAssetsDirectory() + filename);

		ShaderStage current_parsing_stage = NONE;

		std::string line;
		while (getline(file, line)) {
			if (line.find("#stage") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					m_ShaderSources[ShaderStage::VERTEX] = std::pair<ShaderStage, std::stringstream>(); // Init shader source
					m_ShaderSources[ShaderStage::VERTEX].value().first = VERTEX;
					current_parsing_stage = VERTEX;
				}
				else if (line.find("fragment") != std::string::npos) {
					m_ShaderSources[ShaderStage::FRAGMENT] = std::pair<ShaderStage, std::stringstream>(); // Init shader source
					m_ShaderSources[ShaderStage::FRAGMENT].value().first = FRAGMENT; // Init shader source
					current_parsing_stage = FRAGMENT;
				}
				else if (line.find("compute") != std::string::npos) {
					m_ShaderSources[ShaderStage::COMPUTE] = std::pair<ShaderStage, std::stringstream>(); // Init shader source
					m_ShaderSources[ShaderStage::COMPUTE].value().first = COMPUTE; // Init shader source
					current_parsing_stage = COMPUTE;
				}
			}
			else {
				m_ShaderSources[current_parsing_stage].value().second << line << '\n';
			}
		}
	}
}

#pragma once

#include <Visus/Core/Shader.h>
#include <Visus/Platform/ShaderUtils/ShaderCompiler.h>

#include <vulkan/vulkan.h>
#include <map>


namespace Visus {

	constexpr VkShaderStageFlagBits VisusToVulkanShaderStage(ShaderStage stage)
	{
		switch (stage)
		{
		case VERTEX:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case FRAGMENT:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case COMPUTE:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		case NONE:
			assert(false, "Invalid shader stage!");
			break;
		default:
			assert(false, "Invalid shader stage!");
			break;

		}
	}

	class VulkanShader : public Shader
	{
	public:
		VulkanShader();
		VulkanShader(const std::string& filename);
		~VulkanShader();

		std::vector<VkPipelineShaderStageCreateInfo> GetPipelineStageCreateInfos() const { return m_CreateInfos; };

	private:
		void ParseFile(const std::string& filename);
		
	private:
		std::array<std::optional<std::pair<ShaderStage, std::stringstream>>, ShaderStage::MAX_VALUE + 1> m_ShaderSources; // Sources of shaders
		std::map<ShaderStage, std::vector<uint32_t>> m_BinaryData;
		std::vector<VkPipelineShaderStageCreateInfo> m_CreateInfos;
	};
}
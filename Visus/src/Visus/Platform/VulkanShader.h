#pragma once

#include <Visus/Core/Shader.h>
#include <Visus/Platform/ShaderUtils/ShaderCompiler.h>

#include <vulkan/vulkan.h>
#include <spirv_cross/spirv_glsl.hpp>
#include <map>

namespace Motus3D {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader();
		VulkanShader(const std::string& filename);
		~VulkanShader();

		std::vector<VkPipelineShaderStageCreateInfo> GetPipelineStageCreateInfos() { return m_CreateInfos; };
		std::vector<VkPushConstantRange> GetPushConstantRangeCreateInfos() { return m_PushConstantRanges; }

	private:
		void ParseFile(const std::string& filename);

	private:
		std::array<std::optional<std::pair<ShaderStage, std::stringstream>>, ShaderStage::MAX_VALUE + 1> m_ShaderSources; // Sources of shaders
		std::map<ShaderStage, std::vector<uint32_t>> m_BinaryData;
		std::vector<VkPipelineShaderStageCreateInfo> m_CreateInfos;

		std::vector<VkPushConstantRange> m_PushConstantRanges;
	};

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

	constexpr VkFormat VisusToVulkanDataFormat(ShaderDataType format)
	{
		switch (format)
		{
			case ShaderDataType::INT:		return VK_FORMAT_R32_SINT;
			case ShaderDataType::INT2:		return VK_FORMAT_R32G32_SINT;
			case ShaderDataType::INT3:		return VK_FORMAT_R32G32B32_SINT;
			case ShaderDataType::INT4:		return VK_FORMAT_R32G32B32A32_SINT;
			case ShaderDataType::FLOAT:		return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::FLOAT2:	return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::FLOAT3:	return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::FLOAT4:	return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::MAT3:		return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::MAT4:		return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::BOOL:		return VK_FORMAT_R8_UINT;
		}
		VISUS_ASSERT(false, "Unknown shader data type");
	}

}
#include "VulkanShader.h"


#ifdef VISUS_DEBUG
	#define MT_USE_SHADER_COMPILER
	#include <Visus/Platform/ShaderUtils/ShaderUtils.h>
#endif
#include <Visus/Platform/VulkanGraphicsContext.h>

#include <SPIRV-Reflect/spirv_reflect.h>
#include <SPIRV-Reflect/spirv_reflect.c>
#include <shaderc/shaderc.hpp>

#include <fstream>
#include <filesystem>

namespace Motus3D
{
	static constexpr VkDescriptorType SpirvToVulkanDescriptorType(SpvReflectDescriptorType type) 
	{
		switch (type)
		{
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
			return VK_DESCRIPTOR_TYPE_SAMPLER;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
			return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			break;
		case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
			return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
			break;
		default:
			MT_CORE_ASSERT(false, "Invalid SPIR-V descriptor type!");
			break;
		}
	}

	VulkanShader::VulkanShader()
	{
		
	}

	VulkanShader::VulkanShader(const std::string& filename)
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		m_IsValid = true;

#ifdef MT_USE_SHADER_COMPILER
		auto compiler = ShaderUtils::GetCompiler();
		ParseFile(std::forward<const std::string&>(filename));
		if(m_ShaderSources[VERTEX].has_value())
		{
			CompileData compile_data;
			compile_data.name = filename;
			compile_data.stage = VERTEX;
			compile_data.source = m_ShaderSources[VERTEX].value().second.str();
			compile_data.output_dir = "assets/shaders/bin/";

			std::vector<uint32_t> binary = compiler->CompileToSpirv(compile_data, &m_IsValid);
			m_BinaryData.emplace(VERTEX, binary);
		}
		if (m_ShaderSources[FRAGMENT].has_value())
		{
			CompileData compile_data;
			compile_data.name = filename;
			compile_data.stage = FRAGMENT;
			compile_data.source = m_ShaderSources[FRAGMENT].value().second.str();
			compile_data.output_dir = "assets/shaders/bin/";

			std::vector<uint32_t> binary = compiler->CompileToSpirv(compile_data, &m_IsValid);
			m_BinaryData.emplace(FRAGMENT, binary);
		}
		if (m_ShaderSources[COMPUTE].has_value())
		{
			CompileData compile_data;
			compile_data.name = filename;
			compile_data.stage = COMPUTE;
			compile_data.source = m_ShaderSources[COMPUTE].value().second.str();
			compile_data.output_dir = "assets/shaders/bin/";

			std::vector<uint32_t> binary = compiler->CompileToSpirv(compile_data, &m_IsValid);
			m_BinaryData.emplace(COMPUTE, binary);
		}
#else
		std::ifstream read_file_stream;
		for (auto& filepath : std::filesystem::directory_iterator("assets/shaders/bin")) {
			auto i_filename = filepath.path().string().substr(filepath.path().string().find_last_of("/\\") + 1);
			if (i_filename.find(filename) != std::string::npos) {
				int file_length;
				read_file_stream.open(filepath, std::ios::ate | std::ios::binary);
				read_file_stream.seekg(0, std::ios::end);
				file_length = read_file_stream.tellg();
				read_file_stream.close();

				if (i_filename.find(".vert") != std::string::npos) {
					std::vector<uint32_t> read_data(file_length / 4);
					read_file_stream.open(filepath, std::ios::in | std::ios::binary);
					read_file_stream.read((char*)read_data.data(), file_length);
					m_BinaryData.emplace(VERTEX, read_data);
				} 
				else if (i_filename.find(".frag") != std::string::npos) {
					std::vector<uint32_t> read_data(file_length / 4);
					read_file_stream.open(filepath, std::ios::in | std::ios::binary);
					read_file_stream.read((char*)read_data.data(), file_length);
					m_BinaryData.emplace(FRAGMENT, read_data);
				}
				else if (i_filename.find(".comp") != std::string::npos) {
					std::vector<uint32_t> read_data(file_length / 4);
					read_file_stream.open(filepath, std::ios::in | std::ios::binary);
					read_file_stream.read((char*)read_data.data(), file_length);
					m_BinaryData.emplace(COMPUTE, read_data);
				}
				else {
					VISUS_TRACE("Unable to read {0} : no appropriate extension (.vert, .frag...)", i_filename);
					m_IsValid = false;
				}
				read_file_stream.close();
			}
		};
#endif

		if(IsValid()) {
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

				// REFLECTION
				SpvReflectShaderModule reflect_module;
				SpvReflectResult reflect_result = spvReflectCreateShaderModule(shader.second.size() * 4, shader.second.data(), &reflect_module);
				MT_CORE_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

				// Reflect Push Constants
				uint32_t push_constant_count;
				spvReflectEnumeratePushConstants(&reflect_module, &push_constant_count, nullptr);
				std::vector<SpvReflectBlockVariable*> ranges(push_constant_count);
				spvReflectEnumeratePushConstants(&reflect_module, &push_constant_count, ranges.data());

				for (auto& range : ranges)
				{
					VkPushConstantRange push_constant = {};
					push_constant.stageFlags = VisusToVulkanShaderStage(shader.first);
					push_constant.size = range->size;
					push_constant.offset = range->offset;

					m_PushConstantRanges.push_back(push_constant);
				}

				// Reflect Descriptor Sets
				uint32_t descriptor_set_count;
				spvReflectEnumerateDescriptorSets(&reflect_module, &descriptor_set_count, nullptr);
				std::vector<SpvReflectDescriptorSet*> reflect_descriptor_sets(descriptor_set_count);
				spvReflectEnumerateDescriptorSets(&reflect_module, &descriptor_set_count, reflect_descriptor_sets.data());

				for (auto& reflect_set : reflect_descriptor_sets)
				{
					VkDescriptorSetLayoutCreateInfo set_layout_create_info = {};
					set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

					std::vector<VkDescriptorSetLayoutBinding> bindings(reflect_set->binding_count);

					for (int i = 0; i < reflect_set->binding_count; i++)
					{
						VkDescriptorSetLayoutBinding binding = {};
						binding.binding = reflect_set->bindings[i]->binding;
						binding.stageFlags = VisusToVulkanShaderStage(shader.first);
						binding.descriptorCount = reflect_set->bindings[i]->count;
						binding.descriptorType = SpirvToVulkanDescriptorType(reflect_set->bindings[i]->descriptor_type);

						bindings[i] = binding;
					}

					set_layout_create_info.bindingCount = bindings.size();
					set_layout_create_info.pBindings = bindings.data();

					VkDescriptorSetLayout final_set_layout;

					VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device->GetHandle(), &set_layout_create_info, nullptr, &final_set_layout));
					m_DescriptorSetLayouts.push_back(final_set_layout);
				}
			}

			for (auto& src : m_ShaderSources)
			{
				if (src.has_value())
				{
					src.value().second.clear();
				}
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

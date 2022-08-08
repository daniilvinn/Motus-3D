#pragma once

#include <Visus/Core/Pipeline.h>
#include <vulkan/vulkan.h>

#include <Visus/Platform/VulkanShader.h>

namespace Motus3D {
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(PipelineSpecification specification);
		~VulkanPipeline() override;

		VkPipeline GetHandle() { return m_Pipeline; }
		VkPipelineLayout GetLayoutHandle() { return m_Layout; }
		VkPushConstantRange GetPushConstant(const std::string& name) { return m_PushConstantRanges[name]; }

	private:
		VkPipeline m_Pipeline;
		VkPipelineLayout m_Layout;
		// vvv It is specific for each pipeline (not even shader), so should be stored in Pipeline's class
		std::unordered_map<std::string, VkPushConstantRange> m_PushConstantRanges;

		Ref<VulkanShader> m_Shader;

	};
}
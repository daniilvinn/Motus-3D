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

		VkPipeline GetHandle() { m_Pipeline; }

	private:
		VkPipeline m_Pipeline;
		VkPipelineLayout m_Layout;

		Ref<VulkanShader> m_Shader;

	};
}

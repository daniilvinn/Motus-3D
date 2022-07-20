#include "VulkanPipeline.h"

#include "VulkanGraphicsContext.h"

namespace Motus3D
{
	VulkanPipeline::VulkanPipeline(PipelineSpecification specification)
	{
		VkGraphicsPipelineCreateInfo graphics_pipeline_create_info;
		graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	}

	VulkanPipeline::~VulkanPipeline()
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		vkDestroyPipeline(device->GetHandle(), m_Pipeline, nullptr);
	}
}

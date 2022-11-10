#include "VulkanPipeline.h"

#include "VulkanGraphicsContext.h"

namespace Motus3D
{
	VulkanPipeline::VulkanPipeline(PipelineSpecification specification)
	{
		m_Shader = RefAs<VulkanShader>(specification.shader);

		switch (specification.executionModel)
		{
		case PipelineExecutionModel::GRAPHICS:
			m_IsValid = CreateGraphicsPipeline(specification);
			break;
		case PipelineExecutionModel::COMPUTE:
			m_IsValid = CreateComputePipeline(specification);
			break;
		default:
			VISUS_ASSERT(false, "PipelineSpecification: Invalid argument");
			break;
		}

		if (!m_IsValid)
			VISUS_ERROR("Failed to create pipeline \"{0}\"", specification.debugName);

	}

	VulkanPipeline::~VulkanPipeline()
	{
		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice()->GetHandle();
		vkDeviceWaitIdle(device);
		vkDestroyPipeline(device, m_Pipeline, nullptr);
		vkDestroyPipelineLayout(device, m_Layout, nullptr);
		VISUS_TRACE("Pipeline destroyed");
	}

	bool VulkanPipeline::CreateGraphicsPipeline(PipelineSpecification specification)
	{
		if (!m_Shader->IsValid()) {
			return false;
		};

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice()->GetHandle();
		m_Shader = RefAs<VulkanShader>(specification.shader);

		// ==================
		// Vertex input state
		VkVertexInputBindingDescription vertex_input_binding = {};
		vertex_input_binding.binding = 0;
		vertex_input_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertex_input_binding.stride = specification.pDataLayout->GetStride();
		std::vector<VkVertexInputAttributeDescription> vertex_input_attributes;

		// Computing locations
		int previousLocationsWidth = 0;
		for (auto element : specification.pDataLayout->GetElements())
		{
			uint32_t location;
			float locationComputeResult = (float)element.size / 16.0f;
			if (locationComputeResult <= 1.0f)
			{
				location = previousLocationsWidth;
				previousLocationsWidth += 1;
			}
			else
			{
				location = previousLocationsWidth;
				previousLocationsWidth += locationComputeResult;
			}

			vertex_input_attributes.push_back({
				.location = location,
				.binding = 0,
				.format = VisusToVulkanDataFormat(element.format),
				.offset = element.offset
				});
		}

		VkPipelineVertexInputStateCreateInfo vertex_input_state = {};
		vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_state.vertexBindingDescriptionCount = 1;
		vertex_input_state.pVertexBindingDescriptions = &vertex_input_binding;
		vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes.data();
		vertex_input_state.vertexAttributeDescriptionCount = vertex_input_attributes.size();

		// ========================
		// Primitive assembly state
		VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {};
		input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly_state.primitiveRestartEnable = VK_FALSE;

		// =========================
		// Specifying dynamic states
		VkDynamicState dynamic_states[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamic_state = {};
		dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state.pDynamicStates = dynamic_states;
		dynamic_state.dynamicStateCount = 2;

		// ===================
		// Rasterization state
		VkPipelineRasterizationStateCreateInfo rasterization_state = {};
		rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_state.depthClampEnable = VK_FALSE;
		rasterization_state.rasterizerDiscardEnable = VK_FALSE;
		rasterization_state.polygonMode = specification.polygonMode == PolygonMode::FILL ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
		rasterization_state.lineWidth = 1.0f;

		switch (specification.cullMode)
		{
		case CullMode::NONE:
			rasterization_state.cullMode = VK_CULL_MODE_NONE;
			break;
		case CullMode::BACK:
			rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
			break;
		case CullMode::FRONT:
			rasterization_state.cullMode = VK_CULL_MODE_FRONT_BIT;
			break;
		default:
			rasterization_state.cullMode = VK_CULL_MODE_NONE;
			break;
		}
		rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterization_state.depthBiasEnable = VK_FALSE;

		// ==============
		// Viewport state
		std::pair<uint32_t, uint32_t> viewportExtent = VulkanGraphicsContext::GetVulkanContext()->GetSwapchain()->GetExtent();
		VkViewport viewport = {};
		viewport = { 0, (float)viewportExtent.second, (float)viewportExtent.first, -(float)viewportExtent.second, 0.0f, 1.0f };

		VkRect2D scissor = {};
		scissor.extent = { viewportExtent.first, viewportExtent.second };
		scissor.offset = { 0,0 };

		VkPipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;

		// ================
		// Color Blend State
		VkPipelineColorBlendAttachmentState color_blend_attachment = {};
		color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		color_blend_attachment.blendEnable = VK_TRUE;
		color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo color_blend_state = {};
		color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_state.attachmentCount = 1;
		color_blend_state.pAttachments = &color_blend_attachment;
		color_blend_state.logicOpEnable = VK_FALSE;

		// ===================
		// Depth stencil State
		VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {};
		depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil_state.depthTestEnable = specification.depthTestEnabled;
		depth_stencil_state.depthWriteEnable = VK_TRUE;
		depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
		depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
		depth_stencil_state.stencilTestEnable = VK_FALSE;

		// =================
		// Multisample state
		VkPipelineMultisampleStateCreateInfo multisample_state = {};
		multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample_state.sampleShadingEnable = VK_FALSE;
		multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// ===============
		// Pipeline layout
		std::vector<VkPushConstantRange> push_constant_ranges = m_Shader->GetPushConstantRangeCreateInfos();
		std::vector<VkDescriptorSetLayout> set_layouts = m_Shader->GetDescriptorSetLayouts();

		VkPipelineLayoutCreateInfo pipeline_layout = {};
		pipeline_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout.pushConstantRangeCount = push_constant_ranges.size();
		pipeline_layout.pPushConstantRanges = push_constant_ranges.data();
		pipeline_layout.setLayoutCount = set_layouts.size();
		pipeline_layout.pSetLayouts = set_layouts.data();

		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipeline_layout, nullptr, &m_Layout));

		VkFormat image_formats[] = { VK_FORMAT_R16G16B16A16_SFLOAT };

		VkPipelineRenderingCreateInfo pipeline_rendering = {};
		pipeline_rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipeline_rendering.colorAttachmentCount = 1;
		pipeline_rendering.pColorAttachmentFormats = image_formats;
		pipeline_rendering.depthAttachmentFormat = VulkanGraphicsContext::GetVulkanContext()->GetSwapchain()->GetDepthBuffer().format;

		// vvv Dangling pointer fix vvv
		std::vector<VkPipelineShaderStageCreateInfo> stages_create_infos = m_Shader->GetPipelineStageCreateInfos();

		VkGraphicsPipelineCreateInfo pipeline_create_info = {};
		pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_create_info.pNext = &pipeline_rendering;
		pipeline_create_info.pStages = stages_create_infos.data();
		pipeline_create_info.stageCount = stages_create_infos.size();
		pipeline_create_info.pVertexInputState = &vertex_input_state;
		pipeline_create_info.pInputAssemblyState = &input_assembly_state;
		pipeline_create_info.pDynamicState = &dynamic_state;
		pipeline_create_info.pRasterizationState = &rasterization_state;
		pipeline_create_info.pColorBlendState = &color_blend_state;
		pipeline_create_info.pDepthStencilState = &depth_stencil_state;
		pipeline_create_info.pMultisampleState = &multisample_state;
		pipeline_create_info.layout = m_Layout;
		pipeline_create_info.pViewportState = &viewport_state;
		pipeline_create_info.renderPass = VK_NULL_HANDLE;

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &m_Pipeline));

		VISUS_TRACE("Pipeline \"{0}\" created successfully!", specification.debugName);

		for (auto& stage : stages_create_infos)
		{
			vkDestroyShaderModule(device, stage.module, nullptr);
		}

		return true;
	}

	bool VulkanPipeline::CreateComputePipeline(PipelineSpecification specification)
	{
		if (!m_Shader->IsValid()) {
			return false;
		};

		auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
		VISUS_ASSERT(m_Shader->GetPipelineStageCreateInfos().size() == 1, "Shader has more or less than 1 stage!");
		VISUS_ASSERT(m_Shader->GetPipelineStageCreateInfos()[0].stage == VK_SHADER_STAGE_COMPUTE_BIT, "Passed graphics shader, compute shader required!");

		std::vector<VkPushConstantRange> push_constant_ranges = m_Shader->GetPushConstantRangeCreateInfos();
		std::vector<VkDescriptorSetLayout> set_layouts = m_Shader->GetDescriptorSetLayouts();

		VkPipelineLayoutCreateInfo pipeline_layout = {};
		pipeline_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout.pushConstantRangeCount = push_constant_ranges.size();
		pipeline_layout.pPushConstantRanges = push_constant_ranges.data();
		pipeline_layout.setLayoutCount = set_layouts.size();
		pipeline_layout.pSetLayouts = set_layouts.data();

		VK_CHECK_RESULT(vkCreatePipelineLayout(device->GetHandle(), &pipeline_layout, nullptr, &m_Layout));

		VkComputePipelineCreateInfo pipeline_create_info = {};
		pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipeline_create_info.layout = m_Layout;
		pipeline_create_info.stage = m_Shader->GetPipelineStageCreateInfos()[0]; // This is the only stage

		VK_CHECK_RESULT(vkCreateComputePipelines(device->GetHandle(), VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &m_Pipeline));
		VISUS_TRACE("Pipeline \"{0}\" created successfully!", specification.debugName);

		vkDestroyShaderModule(device->GetHandle(), m_Shader->GetPipelineStageCreateInfos()[0].module, nullptr);
		
		return true;
	}

}

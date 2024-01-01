#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanPipeline.h"
#include "ImVulkan/Util/AssetManager.h"

namespace ImVulkan
{
	VulkanPipeline::VulkanPipeline(
		VkDevice device, 
		uint32_t shaderStageCount, 
		VkPipelineShaderStageCreateInfo* shaderStages, 
		VkRenderPass renderPass, 
		VkVertexInputAttributeDescription* attributeDescriptions, 
		uint32_t numAttributeDescriptions, 
		VkVertexInputBindingDescription* bindingDescription,
		uint32_t numSetLayouts,
		VkDescriptorSetLayout* setLayouts
	)
	{
		VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		vertexInputState.vertexBindingDescriptionCount = bindingDescription ? 1 : 0;
		vertexInputState.pVertexBindingDescriptions = bindingDescription;
		vertexInputState.vertexAttributeDescriptionCount = numAttributeDescriptions;
		vertexInputState.pVertexAttributeDescriptions = attributeDescriptions;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizationState.lineWidth = 1.f;

		VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &colorBlendAttachment;

		VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		dynamicState.dynamicStateCount = ARRAY_COUNT(dynamicStates);
		dynamicState.pDynamicStates = dynamicStates;

		{
			VkPipelineLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
			createInfo.setLayoutCount = numSetLayouts;
			createInfo.pSetLayouts = setLayouts;

			VK_ASSERT(vkCreatePipelineLayout(device, &createInfo, nullptr, &m_PipelineLayout), "Could not create pipeline layout!");
		}

		{
			VkGraphicsPipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
			createInfo.stageCount = shaderStageCount;
			createInfo.pStages = shaderStages;
			createInfo.pVertexInputState = &vertexInputState;
			createInfo.pInputAssemblyState = &inputAssemblyState;
			createInfo.pViewportState = &viewportState;
			createInfo.pRasterizationState = &rasterizationState;
			createInfo.pMultisampleState = &multisampleState;
			createInfo.pColorBlendState = &colorBlendState;
			createInfo.pDynamicState = &dynamicState;
			createInfo.layout = m_PipelineLayout;
			createInfo.renderPass = renderPass;
			createInfo.subpass = 0;

			VK_ASSERT(vkCreateGraphicsPipelines(device, nullptr, 1, &createInfo, nullptr, &m_Pipeline), "Could not create graphics pipeline!");
		}

		for (uint32_t i = 0; i < shaderStageCount; i++)
		{
			vkDestroyShaderModule(device, shaderStages[i].module, nullptr);
		}
	}

	VulkanPipeline::VulkanPipeline(VulkanPipeline&& other) noexcept
		: m_Pipeline(other.m_Pipeline), m_PipelineLayout(other.m_PipelineLayout)
	{
		other.m_Pipeline = nullptr;
		other.m_PipelineLayout = nullptr;
	}

	VulkanPipeline& VulkanPipeline::operator=(VulkanPipeline&& other) noexcept
	{
		if (this != &other)
		{
			m_Pipeline = other.m_Pipeline;
			m_PipelineLayout = other.m_PipelineLayout;

			other.m_Pipeline = nullptr;
			other.m_PipelineLayout = nullptr;
		}

		return *this;
	}

	void VulkanPipeline::Destroy(VkDevice device)
	{
		vkDestroyPipeline(device, m_Pipeline, nullptr);
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
	}

	VkShaderModule VulkanPipeline::CreateShaderModule(
		VkDevice device, 
		const char* shaderFilePath
	)
	{
		VkShaderModule shaderModule = {};

		std::string shaderCode = AssetManager::GetFileContent(shaderFilePath);
		
		VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
		VK_ASSERT(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule), "Could not create shader module!");
		
		return shaderModule;
	}
}

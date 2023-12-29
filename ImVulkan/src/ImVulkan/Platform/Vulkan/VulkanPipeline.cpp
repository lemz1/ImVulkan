#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanPipeline.h"
#include "ImVulkan/Util/AssetManager.h"

namespace ImVulkan
{
	VulkanPipeline::VulkanPipeline(VulkanContext& context, uint32_t shaderStageCount, VkPipelineShaderStageCreateInfo* shaderStages,
								   VkRenderPass renderPass, uint32_t width, uint32_t height)
	{
		VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportState.viewportCount = 1;
		VkViewport viewport = { 0.f, 0.f, (float)width, (float)height };
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		VkRect2D scissor = { {0, 0}, {width, height} };
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizationState.lineWidth = 1.f;

		VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &colorBlendAttachment;

		{
			VkPipelineLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
			VK_ASSERT(vkCreatePipelineLayout(context.GetDevice(), &createInfo, nullptr, &m_PipelineLayout), "Could not create pipeline layout!");
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
			createInfo.layout = m_PipelineLayout;
			createInfo.renderPass = renderPass;
			createInfo.subpass = 0;

			VK_ASSERT(vkCreateGraphicsPipelines(context.GetDevice(), nullptr, 1, &createInfo, nullptr, &m_Pipeline), "Could not create graphics pipeline!");
		}

		for (uint32_t i = 0; i < shaderStageCount; i++)
		{
			vkDestroyShaderModule(context.GetDevice(), shaderStages[i].module, nullptr);
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

	void VulkanPipeline::Destroy(VulkanContext& context)
	{
		vkDestroyPipeline(context.GetDevice(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(context.GetDevice(), m_PipelineLayout, nullptr);
	}

	VkShaderModule VulkanPipeline::CreateShaderModule(VulkanContext& context, const char* shaderFilePath)
	{
		VkShaderModule shaderModule = {};

		std::string shaderCode = AssetManager::GetFileContent(shaderFilePath);
		
		VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
		VK_ASSERT(vkCreateShaderModule(context.GetDevice(), &createInfo, nullptr, &shaderModule), "Could not create shader module!");

		return shaderModule;
	}
}

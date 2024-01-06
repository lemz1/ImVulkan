#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"
#include "ImVulkan/Platform/Vulkan/VulkanContext.h"

namespace ImVulkan::VulkanPipeline
{
	VkPipeline Create(
		VkDevice device,
		uint32_t shaderStageCount,
		VkPipelineShaderStageCreateInfo* shaderStages,
		VkRenderPass renderPass,
		uint32_t numAttributeDescriptions,
		VkVertexInputAttributeDescription* attributeDescriptions,
		VkVertexInputBindingDescription* bindingDescription,
		VkPipelineLayout pipelineLayout
	);
}

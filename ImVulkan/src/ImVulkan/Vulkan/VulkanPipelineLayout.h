#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanPipelineLayout
{
	VkPipelineLayout Create(
		VkDevice device,
		uint32_t numSetLayouts,
		VkDescriptorSetLayout* setLayouts
	);
}

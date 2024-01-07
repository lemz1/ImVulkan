#include "imvkpch.h"
#include "ImVulkan/Vulkan/VulkanPipelineLayout.h"

namespace ImVulkan::VulkanPipelineLayout
{
    VkPipelineLayout Create(VkDevice device, uint32_t numSetLayouts, VkDescriptorSetLayout* setLayouts)
    {
		VkPipelineLayout pipelineLayout = nullptr;

		VkPipelineLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		createInfo.setLayoutCount = numSetLayouts;
		createInfo.pSetLayouts = setLayouts;

		VK_ASSERT(
			vkCreatePipelineLayout(
				device,
				&createInfo,
				nullptr,
				&pipelineLayout
			),
			"Could not create pipeline layout!"
		);

		return pipelineLayout;
    }
}

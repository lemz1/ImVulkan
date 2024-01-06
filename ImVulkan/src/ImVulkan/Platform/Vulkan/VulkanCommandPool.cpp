#include "imvkpch.h"
#include "VulkanCommandPool.h"

namespace ImVulkan::VulkanCommandPool
{
	VkCommandPool Create(VkDevice device, uint32_t queueFamilyIndex)
	{
		VkCommandPool commandPool = nullptr;

		VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		createInfo.queueFamilyIndex = queueFamilyIndex;
		VK_ASSERT(
			vkCreateCommandPool(
				device,
				&createInfo,
				nullptr,
				&commandPool
			),
			"Could not create command pool!"
		);

		return commandPool;
	}
}

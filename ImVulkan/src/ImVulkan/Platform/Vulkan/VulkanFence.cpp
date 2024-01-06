#include "imvkpch.h"
#include "VulkanFence.h"

namespace ImVulkan::VulkanFence
{
	VkFence Create(VkDevice device)
	{
		VkFence fence;

		VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VK_ASSERT(
			vkCreateFence(
				device,
				&createInfo,
				nullptr,
				&fence
			), "Could not create fence!"
		);

		return fence;
	}
}

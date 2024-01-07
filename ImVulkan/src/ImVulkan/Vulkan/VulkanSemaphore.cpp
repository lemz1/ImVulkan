#include "imvkpch.h"
#include "ImVulkan/Vulkan/VulkanSemaphore.h"

namespace ImVulkan::VulkanSemaphore
{
	VkSemaphore Create(VkDevice device)
	{
		VkSemaphore semaphore = nullptr;

		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

		VK_ASSERT(
			vkCreateSemaphore(
				device,
				&createInfo,
				nullptr,
				&semaphore
			),
			"Could not create semaphore!"
		);
		
		return semaphore;
	}
}

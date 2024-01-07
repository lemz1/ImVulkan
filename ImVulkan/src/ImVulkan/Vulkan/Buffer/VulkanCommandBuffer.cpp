#include "imvkpch.h"
#include "ImVulkan/Vulkan/Buffer/VulkanCommandBuffer.h"

namespace ImVulkan::VulkanCommandBuffer
{
	VkCommandBuffer Create(
		VkDevice device, 
		VkCommandPool commandPool
	)
	{
		VkCommandBuffer commandBuffer = nullptr;

		VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = commandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = 1;

		VK_ASSERT(
			vkAllocateCommandBuffers(
				device,
				&allocateInfo,
				&commandBuffer
			),
			"Could not allocate command buffer!"
		);

		return commandBuffer;
	}
}

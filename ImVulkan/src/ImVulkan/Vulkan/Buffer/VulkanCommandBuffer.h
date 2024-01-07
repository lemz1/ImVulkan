#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanCommandBuffer
{
	VkCommandBuffer Create(
		VkDevice device, 
		VkCommandPool commandPool
	);
}

#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanCommandBuffer
{
	VkCommandBuffer Create(
		VkDevice device, 
		VkCommandPool commandPool
	);
}

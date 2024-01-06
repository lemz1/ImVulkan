#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanCommandPool
{
	VkCommandPool Create(
		VkDevice device, 
		uint32_t queueFamilyIndex
	);
}

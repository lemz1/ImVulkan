#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanMemory
{
	VkDeviceMemory Create(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		VkMemoryRequirements memoryRequirements,
		VkMemoryPropertyFlags memoryProperties
	);

	uint32_t FindMemoryType(
		VkPhysicalDevice physicalDevice, 
		uint32_t typeFilter, 
		VkMemoryPropertyFlags memoryProperties
	);
}

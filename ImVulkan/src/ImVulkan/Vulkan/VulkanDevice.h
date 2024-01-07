#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanDevice
{
	VkDevice Create(
		VkPhysicalDevice physicalDevice,
		uint32_t queueIndex,
		uint32_t deviceExtensionCount,
		const char** deviceExtensions
	);
}

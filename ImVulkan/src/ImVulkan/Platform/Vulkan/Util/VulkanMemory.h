#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanMemory
{
	uint32_t FindMemoryType(
		VkPhysicalDevice physicalDevice, 
		uint32_t typeFilter, 
		VkMemoryPropertyFlags memoryProperties
	);
}

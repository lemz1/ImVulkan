#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanInstance
{
	VkInstance Create(
		uint32_t instanceExtensionCount, 
		const char** instanceExtensions
	);
}

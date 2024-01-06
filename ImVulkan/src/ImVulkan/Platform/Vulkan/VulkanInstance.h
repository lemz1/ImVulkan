#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanInstance
{
	VkInstance Create(
		uint32_t instanceExtensionCount, 
		const char** instanceExtensions
	);
}

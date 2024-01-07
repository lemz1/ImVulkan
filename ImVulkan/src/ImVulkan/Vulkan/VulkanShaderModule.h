#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanShaderModule
{
	VkShaderModule Create(
		VkDevice device,
		const char* shaderFilePath
	);
}

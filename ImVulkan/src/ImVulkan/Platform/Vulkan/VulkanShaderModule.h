#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanShaderModule
{
	VkShaderModule Create(
		VkDevice device,
		const char* shaderFilePath
	);
}

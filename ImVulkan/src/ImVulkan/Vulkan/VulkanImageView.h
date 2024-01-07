#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanImageView
{
	VkImageView Create(
		VkDevice device,
		VkImage image,
		VkFormat format
	);
}

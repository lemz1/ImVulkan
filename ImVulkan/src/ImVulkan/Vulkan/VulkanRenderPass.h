#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"
#include "ImVulkan/Vulkan/VulkanContext.h"

namespace ImVulkan::VulkanRenderPass
{
	VkRenderPass Create(
		VkDevice device, 
		VkFormat format
	);
}

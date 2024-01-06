#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"
#include "ImVulkan/Platform/Vulkan/VulkanContext.h"

namespace ImVulkan::VulkanRenderPass
{
	VkRenderPass Create(
		VkDevice device, 
		VkFormat format
	);
}

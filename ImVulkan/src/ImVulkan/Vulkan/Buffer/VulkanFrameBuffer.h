#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanFrameBuffer
{
	VkFramebuffer Create(
		VkDevice device, 
		VkRenderPass renderPass, 
		VkImageView imageView, 
		uint32_t width, 
		uint32_t height
	);
}

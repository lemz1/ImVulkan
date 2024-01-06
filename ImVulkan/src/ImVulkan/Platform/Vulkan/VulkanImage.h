#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanImage
{
	VkImage Create(
		VkDevice device, 
		VkPhysicalDevice physicalDevice, 
		uint32_t width, 
		uint32_t height, 
		VkFormat format, 
		VkImageUsageFlags usage
	);

	void UploadDataToImage(
		VkImage image,
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		VkQueue queue,
		uint32_t queueFamilyIndex,
		void* data,
		VkDeviceSize dataSize,
		uint32_t width,
		uint32_t height,
		VkImageLayout finalLayout,
		VkAccessFlags dstAccessMask
	);
}

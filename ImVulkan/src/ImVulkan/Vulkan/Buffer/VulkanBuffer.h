#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanBuffer
{
	VkBuffer Create(
		VkDevice device,
		size_t size,
		VkBufferUsageFlags usage
	);


	void MapMemory(
		VkBuffer buffer,
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		VkQueue queue,
		uint32_t queueFamilyIndex,
		void* data,
		VkDeviceSize dataSize
	);
}

#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"
#include "ImVulkan/Vulkan/VulkanFence.h"
#include "ImVulkan/Vulkan/VulkanDevice.h"
#include "ImVulkan/Vulkan/VulkanInstance.h"
#include "ImVulkan/Vulkan/VulkanPhysicalDevice.h"
#include "ImVulkan/Vulkan/VulkanDebugMessenger.h"

namespace ImVulkan
{
	struct VulkanContext
	{
	public:
		VulkanContext() = default;
		VulkanContext(
			uint32_t instanceExtensionCount, 
			const char** instanceExtensions, 
			uint32_t deviceExtensionCount, 
			const char** deviceExtensions
		);

		void Destroy();
	public:
		VkInstance instance;
		VkPhysicalDevice physicalDevice;
		VkPhysicalDeviceProperties physicalDeviceProperties;
		VkDevice device;
		VkQueue queue;
		uint32_t queueFamilyIndex;
		
		VkFence fence;
	};
}

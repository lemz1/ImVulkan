#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"
#include "ImVulkan/Platform/Vulkan/VulkanFence.h"
#include "ImVulkan/Platform/Vulkan/VulkanDevice.h"
#include "ImVulkan/Platform/Vulkan/VulkanInstance.h"
#include "ImVulkan/Platform/Vulkan/VulkanPhysicalDevice.h"
#include "ImVulkan/Platform/Vulkan/VulkanDebugMessenger.h"

namespace ImVulkan
{
	struct VulkanContext
	{
	public:
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

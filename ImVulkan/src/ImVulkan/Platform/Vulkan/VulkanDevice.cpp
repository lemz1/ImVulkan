#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanDevice.h"

#include "ImVulkan/Platform/Vulkan/VulkanQueueIndex.h"

namespace ImVulkan::VulkanDevice
{
	VkDevice ImVulkan::VulkanDevice::Create(
		VkPhysicalDevice physicalDevice,
		uint32_t queueIndex,
		uint32_t deviceExtensionCount, 
		const char** deviceExtensions
	)
	{
		VkDevice device = nullptr;

		float priorities[] = { 1.f };
		VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		queueCreateInfo.queueFamilyIndex = queueIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = priorities;

		VkPhysicalDeviceFeatures features = {};

		VkDeviceCreateInfo createInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.enabledExtensionCount = deviceExtensionCount;
		createInfo.ppEnabledExtensionNames = deviceExtensions;
		createInfo.pEnabledFeatures = &features;

		VK_ASSERT(
			vkCreateDevice(
				physicalDevice,
				&createInfo,
				nullptr,
				&device
			),
			"Failed to create logical device!"
		);

		return device;
	}
}

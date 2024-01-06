#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanQueueIndex.h"

namespace ImVulkan::VulkanQueueIndex
{
    uint32_t Create(VkPhysicalDevice physicalDevice)
    {
		uint32_t queueFamilyIndex = 0;

		uint32_t numQueueFamilies = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilies, nullptr);
		VkQueueFamilyProperties* queueFamilies = new VkQueueFamilyProperties[numQueueFamilies];
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilies, queueFamilies);

		for (uint32_t i = 0; i < numQueueFamilies; i++)
		{
			VkQueueFamilyProperties queueFamily = queueFamilies[i];
			if (queueFamily.queueCount == 0)
			{
				continue;
			}

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queueFamilyIndex = i;
				break;
			}
		}
		delete[] queueFamilies;

		return queueFamilyIndex;
    }
}

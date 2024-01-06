#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanPhysicalDevice.h"

namespace ImVulkan::VulkanPhysicalDevice
{
	VkPhysicalDevice Create(VkInstance instance)
	{
		VkPhysicalDevice physicalDevice;

		uint32_t numDevices;
		VK_ASSERT(
			vkEnumeratePhysicalDevices(
				instance,
				&numDevices,
				nullptr
			),
			"Error enumerating physical devices!"
		);
		IMVK_ASSERT(numDevices == 0, "Vulkan: Failed to find GPUs with Vulkan Support!");

		VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[numDevices];
		VK_ASSERT(
			vkEnumeratePhysicalDevices(
				instance,
				&numDevices,
				physicalDevices
			),
			"Error enumerating physical devices!"
		);

		#ifdef VK_DEBUG_INFO
		IMVK_INFO("Found " << numDevices << " GPU(s):");
		for (uint32_t i = 0; i < numDevices; i++)
		{
			VkPhysicalDeviceProperties properties = {};
			vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
			IMVK_INFO("GPU: " << i << ": " << properties.deviceName);
		}
		#endif

		physicalDevice = physicalDevices[0];
		delete[] physicalDevices;

		#ifdef VK_DEBUG_INFO
		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		IMVK_INFO("Selected GPU: " << properties.deviceName);
		#endif

		return physicalDevice;
	}
}

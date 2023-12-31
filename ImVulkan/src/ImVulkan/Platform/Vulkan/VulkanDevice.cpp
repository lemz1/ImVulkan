#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanDevice.h"

namespace ImVulkan
{
	VulkanDevice::VulkanDevice(
		VkPhysicalDevice physicalDevice, 
		uint32_t deviceExtensionCount, 
		const char** deviceExtensions
	)
	{
		uint32_t numQueueFamilies = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilies, nullptr);
		VkQueueFamilyProperties* queueFamilies = new VkQueueFamilyProperties[numQueueFamilies];
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilies, queueFamilies);

		uint32_t graphicsQueueIndex = 0;
		for (uint32_t i = 0; i < numQueueFamilies; i++)
		{
			VkQueueFamilyProperties queueFamily = queueFamilies[i];
			if (queueFamily.queueCount == 0)
			{
				continue;
			}

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsQueueIndex = i;
				break;
			}
		}
		delete[] queueFamilies;

		float priorities[] = { 1.f };
		VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		queueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = priorities;

		VkPhysicalDeviceFeatures features = {};

		VkDeviceCreateInfo createInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.enabledExtensionCount = deviceExtensionCount;
		createInfo.ppEnabledExtensionNames = deviceExtensions;
		createInfo.pEnabledFeatures = &features;

		VK_ASSERT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_Device), "Failed to create logical device!");

		m_QueueFamilyIndex = graphicsQueueIndex;
		vkGetDeviceQueue(m_Device, m_QueueFamilyIndex, 0, &m_Queue);

		#ifdef VK_DEBUG_INFO
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
		IMVK_INFO("Num device memory heaps: " << deviceMemoryProperties.memoryHeapCount);
		for (uint32_t i = 0; i < deviceMemoryProperties.memoryHeapCount; i++)
		{
			const char* isDeviceLocal = "false";
			if (deviceMemoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				isDeviceLocal = "true";
			}
			IMVK_INFO("Heap " << i << ": Size: " << ((double)deviceMemoryProperties.memoryHeaps[i].size * .001f * .001f) << "MB, Device local: " << isDeviceLocal);
		}
		#endif
	}

	VulkanDevice::VulkanDevice(VulkanDevice&& other) noexcept
		: m_Device(other.m_Device), m_Queue(other.m_Queue), m_QueueFamilyIndex(other.m_QueueFamilyIndex)
	{
		other.m_Device = nullptr;
		other.m_Queue = nullptr;
		other.m_QueueFamilyIndex = UINT32_MAX;
	}

	VulkanDevice& VulkanDevice::operator=(VulkanDevice&& other) noexcept
	{
		if (this != &other)
		{
			m_Device = other.m_Device;
			m_Queue = other.m_Queue;
			m_QueueFamilyIndex = other.m_QueueFamilyIndex;

			other.m_Device = nullptr;
			other.m_Queue = nullptr;
			other.m_QueueFamilyIndex = UINT32_MAX;
		}

		return *this;
	}

	void VulkanDevice::Destroy()
	{
		vkDestroyDevice(m_Device, nullptr);
	}
}

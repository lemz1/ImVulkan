#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanPhysicalDevice.h"

namespace ImVulkan
{
	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance)
	{
		uint32_t numDevices;
		VK_ASSERT(vkEnumeratePhysicalDevices(instance, &numDevices, nullptr), "Error enumerating physical devices!");
		IMVK_ASSERT(numDevices == 0, "Vulkan: Failed to find GPUs with Vulkan Support!");

		VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[numDevices];
		VK_ASSERT(vkEnumeratePhysicalDevices(instance, &numDevices, physicalDevices), "Error enumerating physical devices!");

		#ifdef VK_DEBUG_INFO
		IMVK_INFO("Found " << numDevices << " GPU(s):");
		for (uint32_t i = 0; i < numDevices; i++)
		{
			VkPhysicalDeviceProperties properties = {};
			vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
			IMVK_INFO("GPU: " << i << ": " << properties.deviceName);
		}
		#endif

		m_PhysicalDevice = physicalDevices[0];
		delete[] physicalDevices;

		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);

		#ifdef VK_DEBUG_INFO
		IMVK_INFO("Selected GPU: " << m_PhysicalDeviceProperties.deviceName);
		#endif
	}

	VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanPhysicalDevice&& other) noexcept
		: m_PhysicalDevice(other.m_PhysicalDevice), m_PhysicalDeviceProperties(other.m_PhysicalDeviceProperties)
	{
		other.m_PhysicalDevice = nullptr;
		other.m_PhysicalDeviceProperties = {};
	}

	VulkanPhysicalDevice& VulkanPhysicalDevice::operator=(VulkanPhysicalDevice&& other) noexcept
	{
		if (this != &other)
		{
			m_PhysicalDevice = other.m_PhysicalDevice;
			m_PhysicalDeviceProperties = other.m_PhysicalDeviceProperties;

			other.m_PhysicalDevice = nullptr;
			other.m_PhysicalDeviceProperties = {};
		}

		return *this;
	}
}

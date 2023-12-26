#include "imvkpch.h"
#include "VulkanContext.h"

#include "ImVulkan/Core/Core.h"

namespace ImVulkan 
{
	VulkanContext::VulkanContext()
	{}

	VulkanContext::~VulkanContext()
	{}

	void VulkanContext::Setup(uint32_t extensionCount, const char** extensions)
	{
		InitVulkanInstance(extensionCount, extensions);

		SelectPhysicalDevice();
	}

	void VulkanContext::InitVulkanInstance(uint32_t extensionCount, const char** extensions)
	{
		uint32_t layerPropertyCount;
		VK_ASSERT(vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr), "Error enumerating instance layer properties!");
		VkLayerProperties* layerProperties = new VkLayerProperties[layerPropertyCount];
		VK_ASSERT(vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties), "Error enumerating instance layer properties!");

		#ifdef VK_DEBUG_INFO
		for (uint32_t i = 0; i < layerPropertyCount; i++)
		{
			IMVK_INFO(layerProperties[i].layerName);
			IMVK_INFO(layerProperties[i].description);
		}
		#endif
		delete[] layerProperties;

		const char* enabledLayers[] =
		{
			"VK_LAYER_KHRONOS_validation",
		};

		//uint32_t availableInstanceExtensionCount;
		//VK_ASSERT(vkEnumerateInstanceExtensionProperties(nullptr, &availableInstanceExtensionCount, nullptr), Error enumerating extension properties!);
		//VkExtensionProperties* availableinstanceExtensionProperties = new VkExtensionProperties[availableInstanceExtensionCount];
		//VK_ASSERT(vkEnumerateInstanceExtensionProperties(nullptr, &availableInstanceExtensionCount, availableinstanceExtensionProperties), Error enumerating extension properties!);
		//#ifdef VK_DEBUG_INFO
		//for (uint32_t i = 0; i < availableInstanceExtensionCount; i++)
		//{
		//	IMVK_INFO(availableinstanceExtensionProperties[i].extensionName);
		//}
		//#endif
		//delete[] availableinstanceExtensionProperties;

		VkApplicationInfo applicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
		applicationInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		createInfo.pApplicationInfo = &applicationInfo;
		createInfo.enabledLayerCount = ARRAY_COUNT(enabledLayers);
		createInfo.ppEnabledLayerNames = enabledLayers;
		createInfo.enabledExtensionCount = extensionCount;
		createInfo.ppEnabledExtensionNames = extensions;

		VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_Instance), "Error creating Vulkan instance!");
	}

	void VulkanContext::SelectPhysicalDevice()
	{
		uint32_t numDevices;
		VK_ASSERT(vkEnumeratePhysicalDevices(m_Instance, &numDevices, nullptr), "Error enumerating physical devices!");
		IMVK_ASSERT(numDevices == 0, "Vulkan: Failed to find GPUs with Vulkan Support!");

		VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[numDevices];
		VK_ASSERT(vkEnumeratePhysicalDevices(m_Instance, &numDevices, physicalDevices), "Error enumerating physical devices!");
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
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);
		#ifdef VK_DEBUG_INFO
		IMVK_INFO("Selected GPU: " << m_PhysicalDeviceProperties.deviceName);
		#endif
		delete[] physicalDevices;
	}
}

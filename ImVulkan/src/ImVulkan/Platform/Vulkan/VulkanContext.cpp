#include "imvkpch.h"
#include "VulkanContext.h"

#include "ImVulkan/Core/Core.h"

namespace ImVulkan 
{
	void VulkanContext::Destroy()
	{
		DestroyDevice();
	}

	void VulkanContext::CreateDevice(uint32_t instanceExtensionCount, const char** instanceExtensions, uint32_t deviceExtensionCount, const char** deviceExtensions)
	{
		InitVulkanInstance(instanceExtensionCount, instanceExtensions);

		SelectPhysicalDevice();

		CreateLogicalDevice(deviceExtensionCount, deviceExtensions);
	}
	
	void VulkanContext::InitVulkanInstance(uint32_t instanceExtensionCount, const char** instanceExtensions)
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
		createInfo.enabledExtensionCount = instanceExtensionCount;
		createInfo.ppEnabledExtensionNames = instanceExtensions;

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

	void VulkanContext::CreateLogicalDevice(uint32_t deviceExtensionCount, const char** deviceExtensions) 
	{
		uint32_t numQueueFamilies = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &numQueueFamilies, nullptr);
		VkQueueFamilyProperties* queueFamilies = new VkQueueFamilyProperties[numQueueFamilies];
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &numQueueFamilies, queueFamilies);

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

		VK_ASSERT(vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device), "Failed to create logical device!");

		m_GraphicsQueue.queueFamilyIndex = graphicsQueueIndex;
		vkGetDeviceQueue(m_Device, m_GraphicsQueue.queueFamilyIndex, 0, &m_GraphicsQueue.queue);
	}
	
	void VulkanContext::DestroyDevice()
	{
		vkDestroyDevice(m_Device, nullptr);

		vkDestroyInstance(m_Instance, nullptr);
	}
}

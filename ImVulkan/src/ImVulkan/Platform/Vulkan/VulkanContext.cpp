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

	}

	void VulkanContext::InitVulkanInstance(uint32_t extensionCount, const char** extensions)
	{
		uint32_t layerPropertyCount;
		VK_ASSERT(vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr), "Vulkan: Error enumerating instance layer properties");
		VkLayerProperties* layerProperties = new VkLayerProperties[layerPropertyCount];
		VK_ASSERT(vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties), "Vulkan: Error enumerating instance layer properties");

		#ifdef VULKAN_DEBUG_INFO
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
		//vkEnumerateInstanceExtensionProperties(nullptr, &availableInstanceExtensionCount, nullptr);
		//VkExtensionProperties* availableinstanceExtensionProperties = new VkExtensionProperties[availableInstanceExtensionCount];
		//vkEnumerateInstanceExtensionProperties(nullptr, &availableInstanceExtensionCount, availableinstanceExtensionProperties);
		//#ifdef VULKAN_DEBUG_INFO
		//for (uint32_t i = 0; i < availableInstanceExtensionCount; i++)
		//{
		//	IMVK_INFO(availableinstanceExtensionProperties[i].extensionName);
		//}
		//#endif
		//delete[] availableinstanceExtensionProperties;

		VkApplicationInfo applicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
		applicationInfo.apiVersion = VK_VERSION_1_3;

		VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		createInfo.pApplicationInfo = &applicationInfo;
		createInfo.enabledLayerCount = ARRAY_COUNT(enabledLayers);
		createInfo.ppEnabledLayerNames = enabledLayers;
		createInfo.enabledExtensionCount = extensionCount;
		createInfo.ppEnabledExtensionNames = extensions;

		VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_Instance), "Vulkan: Error creating Vulkan instance!");
	}
}

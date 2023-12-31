#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanInstance.h"

namespace ImVulkan
{
	VulkanInstance::VulkanInstance(
		uint32_t instanceExtensionCount, 
		const char** instanceExtensions
	)
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

		VkApplicationInfo applicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
		applicationInfo.apiVersion = VK_API_VERSION_1_3;

		VkValidationFeatureEnableEXT enabledValidationFeatures[] = {
			//VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
			#ifdef DEBUG
			VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
			#endif
		};
		VkValidationFeaturesEXT validationFeatures = { VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT };
		validationFeatures.enabledValidationFeatureCount = ARRAY_COUNT(enabledValidationFeatures);
		validationFeatures.pEnabledValidationFeatures = enabledValidationFeatures;

		VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		createInfo.pNext = &validationFeatures;
		createInfo.pApplicationInfo = &applicationInfo;
		createInfo.enabledLayerCount = ARRAY_COUNT(enabledLayers);
		createInfo.ppEnabledLayerNames = enabledLayers;
		createInfo.enabledExtensionCount = instanceExtensionCount;
		createInfo.ppEnabledExtensionNames = instanceExtensions;

		VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_Instance), "Error creating Vulkan instance!");
	}

	VulkanInstance::VulkanInstance(VulkanInstance&& other) noexcept
		: m_Instance(other.m_Instance)
	{
		other.m_Instance = nullptr;
	}

	VulkanInstance& VulkanInstance::operator=(VulkanInstance&& other) noexcept
	{
		if (this != &other)
		{
			m_Instance = other.m_Instance;
		}

		return *this;
	}

	void VulkanInstance::Destroy()
	{
		vkDestroyInstance(m_Instance, nullptr);
	}
}

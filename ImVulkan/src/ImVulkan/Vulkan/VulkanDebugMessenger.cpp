#include "imvkpch.h"
#include "ImVulkan/Vulkan/VulkanDebugMessenger.h"

namespace ImVulkan::VulkanDebugMessenger
{
	PFN_vkCreateDebugUtilsMessengerEXT CreateDebugMessenger;
	PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugMessenger;

	VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData
	)
	{
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			IMVK_ERROR(callbackData->pMessage);
		}
		else
		{
			IMVK_WARN(callbackData->pMessage);
		}
		return VK_FALSE;
	}

	VkDebugUtilsMessengerEXT Create(VkInstance instance)
	{
		VkDebugUtilsMessengerEXT debugMessenger = nullptr;

		if (!CreateDebugMessenger)
		{
			CreateDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		}

		VkDebugUtilsMessengerCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
							   | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
							   | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;

		VK_ASSERT(
			CreateDebugMessenger(
				instance,
				&createInfo,
				nullptr,
				&debugMessenger
			),
			"Could not create debug messenger!"
		);

		return debugMessenger;
	}

	void Destroy(
		VkDebugUtilsMessengerEXT debugMessenger,
		VkInstance instance
	)
	{
		if (!DestroyDebugMessenger)
		{
			DestroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		}

		DestroyDebugMessenger(instance, debugMessenger, nullptr);
	}
}

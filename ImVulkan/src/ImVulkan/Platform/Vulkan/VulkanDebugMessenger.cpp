#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanDebugMessenger.h"

namespace ImVulkan
{
	PFN_vkCreateDebugUtilsMessengerEXT VulkanDebugMessenger::CreateDebugMessenger = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT VulkanDebugMessenger::DestroyDebugMessenger = nullptr;

	VulkanDebugMessenger::VulkanDebugMessenger(VkInstance instance)
	{
		if (!VulkanDebugMessenger::CreateDebugMessenger)
		{
			VulkanDebugMessenger::CreateDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		}

		VkDebugUtilsMessengerCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
							   | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
							   | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		createInfo.pfnUserCallback = VulkanDebugMessenger::DebugCallback;

		VK_ASSERT(
			VulkanDebugMessenger::CreateDebugMessenger(
				instance, 
				&createInfo, 
				nullptr, 
				&m_DebugMessenger
			), 
			"Could not create debug messenger!"
		);
	}

	VulkanDebugMessenger::VulkanDebugMessenger(VulkanDebugMessenger&& other) noexcept
		: m_DebugMessenger(other.m_DebugMessenger)
	{
		other.m_DebugMessenger = nullptr;
	}

	VulkanDebugMessenger& VulkanDebugMessenger::operator=(VulkanDebugMessenger&& other) noexcept
	{
		if (this != &other)
		{
			m_DebugMessenger = other.m_DebugMessenger;
			
			other.m_DebugMessenger = nullptr;
		}

		return *this;
	}

	void VulkanDebugMessenger::Destroy(VkInstance instance)
	{
		if (!VulkanDebugMessenger::DestroyDebugMessenger)
		{
			VulkanDebugMessenger::DestroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		}

		VulkanDebugMessenger::DestroyDebugMessenger(instance, m_DebugMessenger, nullptr);
	}

	VkBool32 VKAPI_CALL VulkanDebugMessenger::DebugCallback(
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
}

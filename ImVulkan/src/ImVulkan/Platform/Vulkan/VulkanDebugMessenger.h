#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanDebugMessenger
{
	VkDebugUtilsMessengerEXT Create(VkInstance instance);

	void Destroy(
		VkDebugUtilsMessengerEXT debugMessenger,
		VkInstance instance
	);
}

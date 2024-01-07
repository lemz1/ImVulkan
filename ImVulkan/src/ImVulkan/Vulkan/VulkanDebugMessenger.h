#pragma once

#include "ImVulkan/Vulkan/VulkanCore.h"

namespace ImVulkan::VulkanDebugMessenger
{
	VkDebugUtilsMessengerEXT Create(VkInstance instance);

	void Destroy(
		VkDebugUtilsMessengerEXT debugMessenger,
		VkInstance instance
	);
}

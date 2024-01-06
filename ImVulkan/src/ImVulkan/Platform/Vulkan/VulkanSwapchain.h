#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"
#include "ImVulkan/Platform/Vulkan/VulkanContext.h"
#include <vector>

namespace ImVulkan
{
	struct VulkanSwapchain
	{
	public:
		VulkanSwapchain(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			uint32_t queueFamilyIndex,
			VkSurfaceKHR surface,
			VkImageUsageFlags usage,
			bool vSync,
			VkSwapchainKHR oldSwapchain = nullptr
		);

		void Destroy(VkDevice device);
	public:
		VkSwapchainKHR swapchain;
		uint32_t width, height;
		VkFormat format;
		VkColorSpaceKHR colorSpace;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
	};
}
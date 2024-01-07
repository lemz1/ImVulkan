#include "imvkpch.h"
#include "ImVulkan/Vulkan/VulkanSwapchain.h"

namespace ImVulkan
{
	VulkanSwapchain::VulkanSwapchain(
		VkDevice device, 
		VkPhysicalDevice physicalDevice, 
		uint32_t queueFamilyIndex, 
		VkSurfaceKHR surface, 
		VkImageUsageFlags usage, 
		bool vSync, 
		VkSwapchainKHR oldSwapchain
	)
	{
		VkBool32 supportsPresent = 0;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &supportsPresent), "Error present not allowed!");

		uint32_t numFormats;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &numFormats, nullptr), "No surface formats available!");
		VkSurfaceFormatKHR* availableFormats = new VkSurfaceFormatKHR[numFormats];
		VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &numFormats, availableFormats), "No surface formats available!");

		// first format should be a sensible default in most cases
		format = availableFormats[0].format;
		colorSpace = availableFormats[0].colorSpace;

		delete[] availableFormats;

		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities), "No surface capabilites!");
		if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
		{
			surfaceCapabilities.currentExtent.width = surfaceCapabilities.minImageExtent.width;
		}

		if (surfaceCapabilities.currentExtent.height == 0xFFFFFFFF)
		{
			surfaceCapabilities.currentExtent.height = surfaceCapabilities.minImageExtent.height;
		}

		if (surfaceCapabilities.maxImageCount == 0)
		{
			surfaceCapabilities.maxImageCount = 8;
		}

		VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		createInfo.surface = surface;
		createInfo.minImageCount = surfaceCapabilities.minImageCount;
		createInfo.imageFormat = format;
		createInfo.imageColorSpace = colorSpace;
		createInfo.imageExtent = surfaceCapabilities.currentExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = usage;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = vSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
		if (oldSwapchain)
		{
			createInfo.oldSwapchain = oldSwapchain;
		}

		width = surfaceCapabilities.currentExtent.width;
		height = surfaceCapabilities.currentExtent.height;

		VK_ASSERT(
			vkCreateSwapchainKHR(
				device, 
				&createInfo, 
				nullptr, 
				&swapchain
			), 
			"failed at creating swapchain"
		);

		uint32_t numImages;
		VK_ASSERT(
			vkGetSwapchainImagesKHR(
				device, 
				swapchain, 
				&numImages, 
				nullptr
			), 
			"failed at swapchain images!"
		);

		images.resize(numImages);

		VK_ASSERT(
			vkGetSwapchainImagesKHR(
				device, 
				swapchain, 
				&numImages, 
				images.data()
			), 
			"failed at swapchain images!"
		);

		imageViews.resize(numImages);
		for (uint32_t i = 0; i < numImages; i++)
		{
			VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			createInfo.image = images[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = format;
			createInfo.components = {};
			createInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

			VK_ASSERT(
				vkCreateImageView(
					device, 
					&createInfo, 
					nullptr, 
					&imageViews[i]
				), 
				"Could not create image views"
			);
		}
	}

	void VulkanSwapchain::Destroy(VkDevice device)
	{
		for (uint32_t i = 0; i < imageViews.size(); i++)
		{
			vkDestroyImageView(device, imageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(device, swapchain, nullptr);
	}
}

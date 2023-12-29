#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanSwapchain.h"

namespace ImVulkan
{
	VulkanSwapchain::VulkanSwapchain(VulkanContext& context, VkSurfaceKHR surface, VkImageUsageFlags usage)
		: m_Surface(surface)
	{
		VkBool32 supportsPresent = 0;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(context.GetPhysicalDevice(), context.GetGraphicsQueue().queueFamilyIndex, 
			m_Surface, &supportsPresent), "Error present not allowed!");

		uint32_t numFormats;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(context.GetPhysicalDevice(), m_Surface, &numFormats, nullptr), "No m_Surface formats available!");
		VkSurfaceFormatKHR* availableFormats = new VkSurfaceFormatKHR[numFormats];
		VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(context.GetPhysicalDevice(), m_Surface, &numFormats, availableFormats), "No m_Surface formats available!");

		// first format should be a sensible default in most cases
		VkFormat format = availableFormats[0].format;
		VkColorSpaceKHR colorSpace = availableFormats[0].colorSpace;

		delete[] availableFormats;

		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.GetPhysicalDevice(), m_Surface, &surfaceCapabilities), "No m_Surface capabilites!");
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
		createInfo.surface = m_Surface;
		createInfo.minImageCount = surfaceCapabilities.minImageCount;
		createInfo.imageFormat = format;
		createInfo.imageColorSpace = colorSpace;
		createInfo.imageExtent = surfaceCapabilities.currentExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = usage;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

		m_Format = format;
		m_Width = surfaceCapabilities.currentExtent.width;
		m_Height = surfaceCapabilities.currentExtent.height;

		VK_ASSERT(vkCreateSwapchainKHR(context.GetDevice(), &createInfo, nullptr, &m_Swapchain), "failed at creating swapchain");

		uint32_t numImages;
		VK_ASSERT(vkGetSwapchainImagesKHR(context.GetDevice(), m_Swapchain, &numImages, nullptr), "failed at swapchain images!");

		m_Images.resize(numImages);

		VK_ASSERT(vkGetSwapchainImagesKHR(context.GetDevice(), m_Swapchain, &numImages, m_Images.data()), "failed at swapchain images!");

		m_ImageViews.resize(numImages);
		for (uint32_t i = 0; i < numImages; i++)
		{
			VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			createInfo.image = m_Images[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_Format;
			createInfo.components = {};
			createInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

			VK_ASSERT(vkCreateImageView(context.GetDevice(), &createInfo, nullptr, &m_ImageViews[i]), "Could not create image views");
		}
	}

	VulkanSwapchain::VulkanSwapchain(VulkanSwapchain&& other) noexcept
		: m_Format(other.m_Format), m_Height(other.m_Height), 
		  m_Images(Move(other.m_Images)), m_ImageViews(Move(other.m_ImageViews)), m_Surface(other.m_Surface),
		  m_Swapchain(other.m_Swapchain), m_Width(other.m_Width)
	{
		other.m_Format = VK_FORMAT_UNDEFINED;
		other.m_Height = 0;
		other.m_Surface = nullptr;
		other.m_Swapchain = nullptr;
		other.m_Width = 0;
	}

	VulkanSwapchain& VulkanSwapchain::operator=(VulkanSwapchain&& other) noexcept
	{
		if (this != &other)
		{
			m_Format = other.m_Format;
			m_Height = other.m_Height;
			m_Images = Move(other.m_Images);
			m_ImageViews = Move(other.m_ImageViews);
			m_Surface = other.m_Surface;
			m_Swapchain = other.m_Swapchain;
			m_Width = other.m_Width;

			other.m_Format = VK_FORMAT_UNDEFINED;
			other.m_Height = 0;
			other.m_Surface = nullptr;
			other.m_Swapchain = nullptr;
			other.m_Width = 0;
		}

		return *this;
	}

	void VulkanSwapchain::Destroy(VulkanContext& context)
	{
		for (uint32_t i = 0; i < m_ImageViews.size(); i++)
		{
			vkDestroyImageView(context.GetDevice(), m_ImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(context.GetDevice(), m_Swapchain, nullptr);
		vkDestroySurfaceKHR(context.GetInstance(), m_Surface, nullptr);
	}
}

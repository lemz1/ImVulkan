#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"
#include "ImVulkan/Platform/Vulkan/VulkanContext.h"
#include <vector>

namespace ImVulkan
{
	class VulkanSwapchain
	{
	public:
		VulkanSwapchain() = default;
		VulkanSwapchain(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			uint32_t queueFamilyIndex,
			VkSurfaceKHR surface,
			VkImageUsageFlags usage,
			bool vSync,
			VkSwapchainKHR oldSwapchain = nullptr
		);

		VulkanSwapchain(const VulkanSwapchain& other) noexcept = delete; // for now no copying

		VulkanSwapchain& operator=(const VulkanSwapchain& other) noexcept = delete;  // for now no copying

		VulkanSwapchain(VulkanSwapchain&& other) noexcept;

		VulkanSwapchain& operator=(VulkanSwapchain&& other) noexcept;

		void Destroy(VkDevice device);

		const VkSwapchainKHR& GetSwapchain() const { return m_Swapchain; }
		const VkFormat GetFormat() const { return m_Format; }
		const uint32_t GetWidth() const { return m_Width; }
		const uint32_t GetHeight() const { return m_Height; }
		const std::vector<VkImage>& GetImages() const { return m_Images; }
		const std::vector<VkImageView>& GetImageViews() const { return m_ImageViews; }
	private:
		VkSwapchainKHR m_Swapchain = nullptr;
		uint32_t m_Width = 0, m_Height = 0;
		VkFormat m_Format = VK_FORMAT_UNDEFINED;
		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
	};
}
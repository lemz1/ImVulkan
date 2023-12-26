#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#define VK_ASSERT(value, message) IMVK_ASSERT(value != VK_SUCCESS, "Vulkan: " << message)

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

#define VK_DEBUG_INFO

namespace ImVulkan
{
	struct VulkanQueue
	{
		VkQueue queue;
		uint32_t queueIndex;
	};

	struct VulkanSwapchain
	{
		VkSwapchainKHR swapchain;
		uint32_t width, height;
		VkFormat format;
		std::vector<VkImage> images;
	};

	class VulkanContext
	{
	public:
		VulkanContext();
		~VulkanContext();

		// --------------- Vulkan Device ----------------------------------------------------
		void CreateDevice(uint32_t instanceExtensionCount, const char** instanceExtensions, uint32_t deviceExtensionCount, const char** deviceExtensions);
		// ----------------------------------------------------------------------------------


		// ------------- Vulkan Swap Chain --------------------------------------------------
		void CreateSwapchain(VkSurfaceKHR surface, VkImageUsageFlags usage);
		// ----------------------------------------------------------------------------------
		
		const VkInstance& GetInstance() { return m_Instance; }
	private:
		// --------------- Vulkan Device ----------------------------------------------------
		void InitVulkanInstance(uint32_t instanceExtensionCount, const char** instanceExtensions);

		void SelectPhysicalDevice();

		void CreateLogicalDevice(uint32_t deviceExtensionCount, const char** deviceExtensions);

		void DestroyDevice();
		// ----------------------------------------------------------------------------------


		// ------------- Vulkan Swap Chain --------------------------------------------------
		void DestroySwapchain();
		// ----------------------------------------------------------------------------------
	private:
		VkInstance m_Instance = nullptr;

		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};
		VkDevice m_Device = nullptr;
		VulkanQueue m_GraphicsQueue = {};
		
		VulkanSwapchain m_Swapchain = {};
		VkSurfaceKHR m_Surface = nullptr;
	};
}

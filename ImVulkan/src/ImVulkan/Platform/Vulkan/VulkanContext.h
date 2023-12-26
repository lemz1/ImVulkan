#pragma once

#include <vulkan/vulkan.h>

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

	class VulkanContext
	{
	public:
		VulkanContext();
		~VulkanContext();

		void Setup(uint32_t extensionCount, const char** extensions);
	private:
		void InitVulkanInstance(uint32_t extensionCount, const char** extensions);

		void SelectPhysicalDevice();

		void CreateLogicalDevice();
	private:
		VkInstance m_Instance = nullptr;
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};
		VkDevice m_Device = nullptr;
		VulkanQueue m_GraphicsQueue = { nullptr, 0 };
	};
}

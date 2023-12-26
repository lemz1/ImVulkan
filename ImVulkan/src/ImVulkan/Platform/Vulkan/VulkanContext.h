#pragma once

#include <vulkan/vulkan.h>

#define VK_ASSERT(value, message) IMVK_ASSERT(value != VK_SUCCESS, message)

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

#define VULKAN_DEBUG_INFO

namespace ImVulkan
{
	class VulkanContext
	{
	public:
		VulkanContext();
		~VulkanContext();

		void Setup(uint32_t extensionCount, const char** extensions);
	private:
		void InitVulkanInstance(uint32_t extensionCount, const char** extensions);
	private:
		VkInstance m_Instance = nullptr;
	};
}

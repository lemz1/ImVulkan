#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanDevice
	{
	public:
		VulkanDevice() = default;
		VulkanDevice(VkPhysicalDevice physicalDevice, uint32_t deviceExtensionCount, const char** deviceExtensions);

		VulkanDevice(const VulkanDevice& other) noexcept = delete; // for now no copying

		VulkanDevice& operator=(const VulkanDevice& other) noexcept = delete;  // for now no copying

		VulkanDevice(VulkanDevice&& other) noexcept;

		VulkanDevice& operator=(VulkanDevice&& other) noexcept;

		void Destroy();

		const VkDevice& GetDevice() const { return m_Device; }
		const VkQueue& GetQueue() const { return m_Queue; }
		const uint32_t GetQueueFamilyIndex() const { return m_QueueFamilyIndex; }
	private:
		VkDevice m_Device = nullptr;

		VkQueue m_Queue = nullptr;
		uint32_t m_QueueFamilyIndex = UINT32_MAX;
	};
}

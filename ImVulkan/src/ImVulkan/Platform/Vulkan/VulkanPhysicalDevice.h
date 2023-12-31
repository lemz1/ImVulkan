#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice() = default;
		VulkanPhysicalDevice(VkInstance instance);

		VulkanPhysicalDevice(const VulkanPhysicalDevice& other) noexcept = delete; // for now no copying

		VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice& other) noexcept = delete;  // for now no copying

		VulkanPhysicalDevice(VulkanPhysicalDevice&& other) noexcept;

		VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&& other) noexcept;

		const VkPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
		const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return m_PhysicalDeviceProperties; }
	private:
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};
	};
}

#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"
#include "ImVulkan/Platform/Vulkan/VulkanFence.h"
#include "ImVulkan/Platform/Vulkan/VulkanDevice.h"
#include "ImVulkan/Platform/Vulkan/VulkanInstance.h"
#include "ImVulkan/Platform/Vulkan/VulkanPhysicalDevice.h"
#include "ImVulkan/Platform/Vulkan/VulkanDebugMessenger.h"

namespace ImVulkan
{
	class VulkanContext
	{
	public:
		VulkanContext() = default;
		VulkanContext(
			uint32_t instanceExtensionCount, 
			const char** instanceExtensions, 
			uint32_t deviceExtensionCount, 
			const char** deviceExtensions
		);

		void InitDebugMessenger();

		void Destroy();

		const VkInstance& GetInstance() const { return m_Instance.GetInstance(); }

		const VkPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice.GetPhysicalDevice(); }
		const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return m_PhysicalDevice.GetPhysicalDeviceProperties(); }

		const VkDevice& GetDevice() const { return m_Device.GetDevice(); }
		const VkQueue& GetQueue() const { return m_Device.GetQueue(); }
		const uint32_t GetQueueFamilyIndex() const { return m_Device.GetQueueFamilyIndex(); }

		const VkFence& GetFence() const { return m_Fence.GetFence(); }
	private:
		VulkanInstance m_Instance;
		VulkanPhysicalDevice m_PhysicalDevice;
		VulkanDevice m_Device;
		VulkanFence m_Fence;
		
		VulkanDebugMessenger m_DebugMessenger;
	};
}

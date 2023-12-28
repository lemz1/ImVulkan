#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"
#include <vector>

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
		VulkanContext() = default;
		void Destroy();

		void CreateDevice(uint32_t instanceExtensionCount, const char** instanceExtensions, uint32_t deviceExtensionCount, const char** deviceExtensions);
		
		VkInstance& GetInstance() { return m_Instance; }
		VkPhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }
		VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() { return m_PhysicalDeviceProperties; }
		VkDevice& GetDevice() { return m_Device; }
		VulkanQueue& GetGraphicsQueue() { return m_GraphicsQueue; }

		const VkInstance& GetInstance() const { return m_Instance; }
		const VkPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
		const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return m_PhysicalDeviceProperties; }
		const VkDevice& GetDevice() const { return m_Device; }
		const VulkanQueue& GetGraphicsQueue() const { return m_GraphicsQueue; }
	private:
		void InitVulkanInstance(uint32_t instanceExtensionCount, const char** instanceExtensions);

		void SelectPhysicalDevice();

		void CreateLogicalDevice(uint32_t deviceExtensionCount, const char** deviceExtensions);

		void DestroyDevice();
	private:
		VkInstance m_Instance = nullptr;

		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};
		VkDevice m_Device = nullptr;
		VulkanQueue m_GraphicsQueue = {};
	};
}

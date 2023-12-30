#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer() = default;
		VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);

		VulkanBuffer(const VulkanBuffer& other) noexcept = delete; // for now no copying

		VulkanBuffer& operator=(const VulkanBuffer& other) noexcept = delete;  // for now no copying

		VulkanBuffer(VulkanBuffer&& other) noexcept;

		VulkanBuffer& operator=(VulkanBuffer&& other) noexcept;

		void Destroy(VkDevice device);

		void MapMemory(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue queue, uint32_t queueFamilyIndex, void* data, VkDeviceSize dataSize);
		void UnmapMemory(VkDevice device);

		const VkBuffer& GetBuffer() const { return m_Buffer; }
		const VkDeviceMemory GetMemory() const { return m_Memory; }
	private:
		uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryProperties);
	private:
		VkBuffer m_Buffer = nullptr;
		VkDeviceMemory m_Memory = nullptr;
	};
}

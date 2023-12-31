#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool() = default;
		VulkanCommandPool(
			VkDevice device, 
			uint32_t queueFamilyIndex
		);

		VulkanCommandPool(const VulkanCommandPool& other) noexcept = delete; // for now no copying

		VulkanCommandPool& operator=(const VulkanCommandPool& other) noexcept = delete;  // for now no copying

		VulkanCommandPool(VulkanCommandPool&& other) noexcept;

		VulkanCommandPool& operator=(VulkanCommandPool&& other) noexcept;

		void Destroy(VkDevice device) const;

		void Reset(VkDevice device) const;

		const VkCommandPool& GetCommandPool() { return m_CommandPool; }
	private:
		VkCommandPool m_CommandPool = nullptr;
	};
}

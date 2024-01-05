#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanSemaphore
	{
	public:
		VulkanSemaphore() = default;
		VulkanSemaphore(VkDevice device);

		VulkanSemaphore(const VulkanSemaphore& other) noexcept = delete; // for now no copying

		VulkanSemaphore& operator=(const VulkanSemaphore& other) noexcept = delete;  // for now no copying

		VulkanSemaphore(VulkanSemaphore&& other) noexcept;

		VulkanSemaphore& operator=(VulkanSemaphore&& other) noexcept;

		void Destroy(VkDevice device);

		const VkSemaphore& GetSemaphore() const { return m_Semaphore; }
	private:
		VkSemaphore m_Semaphore = nullptr;
	};
}

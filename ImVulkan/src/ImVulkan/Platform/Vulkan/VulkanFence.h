#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanFence
	{
	public:
		VulkanFence() = default;
		VulkanFence(VkDevice device);

		VulkanFence(const VulkanFence& other) noexcept = delete; // for now no copying

		VulkanFence& operator=(const VulkanFence& other) noexcept = delete;  // for now no copying

		VulkanFence(VulkanFence&& other) noexcept;

		VulkanFence& operator=(VulkanFence&& other) noexcept;

		void Destroy(VkDevice device);

		void Wait(VkDevice device) const;

		void Reset(VkDevice device) const;

		const VkFence& GetFence() { return m_Fence; }
	private:
		VkFence m_Fence = nullptr;
	};
}

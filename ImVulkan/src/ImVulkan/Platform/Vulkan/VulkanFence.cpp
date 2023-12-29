#include "imvkpch.h"
#include "VulkanFence.h"

namespace ImVulkan
{
	VulkanFence::VulkanFence(VkDevice device)
	{
		VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VK_ASSERT(vkCreateFence(device, &createInfo, nullptr, &m_Fence), "Could not create fence!");
	}

	VulkanFence::VulkanFence(VulkanFence&& other) noexcept
		: m_Fence(other.m_Fence)
	{
		other.m_Fence = nullptr;
	}

	VulkanFence& VulkanFence::operator=(VulkanFence&& other) noexcept
	{
		if (this != &other)
		{
			m_Fence = other.m_Fence;

			other.m_Fence = nullptr;
		}

		return *this;
	}

	void VulkanFence::Destroy(VkDevice device)
	{
		vkDestroyFence(device, m_Fence, nullptr);
	}

	void VulkanFence::Wait(VkDevice device)
	{
		VK_ASSERT(vkWaitForFences(device, 1, &m_Fence, VK_TRUE, UINT64_MAX), "Could not wait for fence!");
	}

	void VulkanFence::Reset(VkDevice device)
	{
		VK_ASSERT(vkResetFences(device, 1, &m_Fence), "Could not reset fence!");
	}
}

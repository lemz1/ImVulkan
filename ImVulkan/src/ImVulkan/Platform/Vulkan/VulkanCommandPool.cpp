#include "imvkpch.h"
#include "VulkanCommandPool.h"

namespace ImVulkan
{
	VulkanCommandPool::VulkanCommandPool(VkDevice device, uint32_t queueFamilyIndex)
	{
		VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		createInfo.queueFamilyIndex = queueFamilyIndex;
		VK_ASSERT(vkCreateCommandPool(device, &createInfo, nullptr, &m_CommandPool), "Could not create command pool!");
	}

	VulkanCommandPool::VulkanCommandPool(VulkanCommandPool&& other) noexcept
		: m_CommandPool(other.m_CommandPool)
	{
		other.m_CommandPool = nullptr;
	}

	VulkanCommandPool& VulkanCommandPool::operator=(VulkanCommandPool&& other) noexcept
	{
		if (this != &other)
		{
			m_CommandPool = other.m_CommandPool;

			other.m_CommandPool = nullptr;
		}

		return *this;
	}

	void VulkanCommandPool::Destroy(VkDevice device)
	{
		vkDestroyCommandPool(device, m_CommandPool, nullptr);
	}

	void VulkanCommandPool::Reset(VkDevice device)
	{
		VK_ASSERT(vkResetCommandPool(device, m_CommandPool, 0), "Could not reset command pool!");
	}
}

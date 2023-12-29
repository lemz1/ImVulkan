#include "imvkpch.h"
#include "VulkanSemaphore.h"

namespace ImVulkan
{
	VulkanSemaphore::VulkanSemaphore(VkDevice device)
	{
		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VK_ASSERT(vkCreateSemaphore(device, &createInfo, nullptr, &m_Semaphore), "Could not create semaphore!");
	}

	VulkanSemaphore::VulkanSemaphore(VulkanSemaphore&& other) noexcept
		: m_Semaphore(other.m_Semaphore)
	{
		other.m_Semaphore = nullptr;
	}

	VulkanSemaphore& VulkanSemaphore::operator=(VulkanSemaphore&& other) noexcept
	{
		if (this != &other)
		{
			m_Semaphore = other.m_Semaphore;

			other.m_Semaphore = nullptr;
		}

		return *this;
	}

	void VulkanSemaphore::Destroy(VkDevice device)
	{
		vkDestroySemaphore(device, m_Semaphore, nullptr);
	}
}

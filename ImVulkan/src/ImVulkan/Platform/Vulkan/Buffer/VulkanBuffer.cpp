#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/Buffer/VulkanBuffer.h"

namespace ImVulkan
{
	VulkanBuffer::VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties)
	{
		{
			VkBufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			createInfo.size = size;
			createInfo.usage = usage;

			VK_ASSERT(vkCreateBuffer(device, &createInfo, nullptr, &m_Buffer), "Could not create vertex buffer!");
		}

		{
			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);

			uint32_t memoryIndex = FindMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, memoryProperties);

			VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			allocateInfo.allocationSize = memoryRequirements.size;
			allocateInfo.memoryTypeIndex = memoryIndex;

			VK_ASSERT(vkAllocateMemory(device, &allocateInfo, nullptr, &m_Memory), "Could not allocate memory!");
		}

		{
			VK_ASSERT(vkBindBufferMemory(device, m_Buffer, m_Memory, 0), "Could not bind buffer memory!");
		}
	}

	VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
		: m_Buffer(other.m_Buffer), m_Memory(other.m_Memory)
	{
		other.m_Buffer = nullptr;
		other.m_Memory = nullptr;
	}

	VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer && other) noexcept
	{
		if (this != &other)
		{
			m_Buffer = other.m_Buffer;
			m_Memory = other.m_Memory;

			other.m_Buffer = nullptr;
			other.m_Memory = nullptr;
		}

		return *this;
	}

	void VulkanBuffer::Destroy(VkDevice device)
	{
		vkDestroyBuffer(device, m_Buffer, nullptr);
		// assumes that the buffer owns its own memory block
		vkFreeMemory(device, m_Memory, nullptr);
	}

	uint32_t VulkanBuffer::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags memoryProperties)
	{
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

		for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) != 0)
			{
				if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
				{
					return i;
				}
			}
		}

		IMVK_ASSERT(true, "Error when trying to find memory type!");
		return UINT32_MAX;
	}
}

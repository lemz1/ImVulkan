#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/Buffer/VulkanBuffer.h"

#include "ImVulkan/Platform/Vulkan/VulkanCommandPool.h"
#include "ImVulkan/Platform/Vulkan/Buffer/VulkanCommandBuffer.h"
#include "ImVulkan/Platform/Vulkan/Util/VulkanMemory.h"

namespace ImVulkan
{
	VulkanBuffer::VulkanBuffer(
		VkDevice device, 
		VkPhysicalDevice physicalDevice, 
		size_t size, 
		VkBufferUsageFlags usage, 
		VkMemoryPropertyFlags memoryProperties
	)
	{
		{
			VkBufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			createInfo.size = size;
			createInfo.usage = usage;

			VK_ASSERT(
				vkCreateBuffer(
					device, 
					&createInfo, 
					nullptr, 
					&m_Buffer), 
				"Could not create vertex buffer!"
			);
		}

		{
			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(
				device, 
				m_Buffer, 
				&memoryRequirements
			);

			uint32_t memoryIndex = VulkanMemory::FindMemoryType(
				physicalDevice, 
				memoryRequirements.memoryTypeBits, 
				memoryProperties
			);

			VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			allocateInfo.allocationSize = memoryRequirements.size;
			allocateInfo.memoryTypeIndex = memoryIndex;

			VK_ASSERT(
				vkAllocateMemory(
					device, 
					&allocateInfo, 
					nullptr, 
					&m_Memory
				), 
				"Could not allocate memory!"
			);
		}

		{
			VK_ASSERT(
				vkBindBufferMemory(
					device, 
					m_Buffer, 
					m_Memory, 
					0
				), 
				"Could not bind buffer memory!"
			);
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

	void VulkanBuffer::MapMemory(
		VkDevice device, 
		VkPhysicalDevice physicalDevice, 
		VkQueue queue, 
		uint32_t queueFamilyIndex, 
		void* data, 
		VkDeviceSize dataSize
	)
	{
		VulkanBuffer stagingBuffer = VulkanBuffer(
			device, 
			physicalDevice, 
			dataSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		{
			void* mapped;
			VK_ASSERT(
				vkMapMemory(
					device, 
					stagingBuffer.m_Memory, 
					0, 
					dataSize, 
					0, 
					&mapped
				), 
				"Could not map memory!"
			);
			memcpy(mapped, data, dataSize);
			stagingBuffer.UnmapMemory(device);
		}

		VulkanCommandPool commandPool = VulkanCommandPool(device, queueFamilyIndex);
		VulkanCommandBuffer commandBuffer = VulkanCommandBuffer(device, commandPool.GetCommandPool());

		{
			commandBuffer.BeginCommandBuffer();

			VkBufferCopy region = { 0, 0, dataSize };
			vkCmdCopyBuffer(
				commandBuffer.GetCommandBuffer(), 
				stagingBuffer.GetBuffer(), 
				m_Buffer, 
				1, 
				&region
			);

			commandBuffer.EndCommandBuffer();
		}

		{
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer.GetCommandBuffer();
			VK_ASSERT(
				vkQueueSubmit(
					queue, 
					1, 
					&submitInfo, 
					nullptr
				), 
				"Could not submit queue!"
			);
			VK_ASSERT(vkQueueWaitIdle(queue), "Could not wait for queue!");
		}

		commandPool.Destroy(device);
		stagingBuffer.Destroy(device);
	}

	void VulkanBuffer::UnmapMemory(VkDevice device)
	{
		vkUnmapMemory(device, m_Memory);
	}
}

#include "imvkpch.h"
#include "ImVulkan/Vulkan/Buffer/VulkanBuffer.h"

#include "ImVulkan/Vulkan/VulkanCommandPool.h"
#include "ImVulkan/Vulkan/Buffer/VulkanCommandBuffer.h"
#include "ImVulkan/Vulkan/VulkanMemory.h"

namespace ImVulkan::VulkanBuffer
{
	VkBuffer Create(
		VkDevice device, 
		size_t size, 
		VkBufferUsageFlags usage
	)
	{
		VkBuffer buffer = nullptr;

		VkBufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		createInfo.size = size;
		createInfo.usage = usage;

		VK_ASSERT(
			vkCreateBuffer(
				device,
				&createInfo,
				nullptr,
				&buffer),
			"Could not create vertex buffer!"
		);

		return buffer;
	}

	void VulkanBuffer::MapMemory(
		VkBuffer buffer,
		VkDevice device, 
		VkPhysicalDevice physicalDevice, 
		VkQueue queue, 
		uint32_t queueFamilyIndex, 
		void* data, 
		VkDeviceSize dataSize
	)
	{
		VkBuffer stagingBuffer = Create(
			device, 
			dataSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		);

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(
			device,
			stagingBuffer,
			&memoryRequirements
		);

		VkDeviceMemory stagingMemory = VulkanMemory::Create(
			physicalDevice,
			device,
			memoryRequirements,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		{
			void* mapped;
			VK_ASSERT(
				vkMapMemory(
					device, 
					stagingMemory,
					0, 
					dataSize, 
					0, 
					&mapped
				), 
				"Could not map memory!"
			);
			memcpy(mapped, data, dataSize);
			vkUnmapMemory(device, stagingMemory);

			vkBindBufferMemory(
				device,
				stagingBuffer,
				stagingMemory,
				0
			);
		}

		VkCommandPool commandPool = VulkanCommandPool::Create(device, queueFamilyIndex);
		VkCommandBuffer commandBuffer = VulkanCommandBuffer::Create(device, commandPool);
		{
			VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			
			VK_ASSERT(
				vkBeginCommandBuffer(
					commandBuffer, 
					&beginInfo
				),
				"Could not begin command buffer!"
			);

			VkBufferCopy region = { 0, 0, dataSize };
			vkCmdCopyBuffer(
				commandBuffer, 
				stagingBuffer, 
				buffer, 
				1, 
				&region
			);

			vkEndCommandBuffer(commandBuffer);
		}

		{
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;
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

		vkDestroyCommandPool(device, commandPool, nullptr);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingMemory, nullptr);
	}
}

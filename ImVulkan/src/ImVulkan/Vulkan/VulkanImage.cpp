#include "imvkpch.h"
#include "ImVulkan/Vulkan/VulkanImage.h"

#include "ImVulkan/Vulkan/Buffer/VulkanBuffer.h"
#include "ImVulkan/Vulkan/VulkanMemory.h"
#include "ImVulkan/Vulkan/VulkanCommandPool.h"
#include "ImVulkan/Vulkan/Buffer/VulkanCommandBuffer.h"

namespace ImVulkan
{
	VkImage VulkanImage::Create(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage)
	{
		VkImage image = nullptr;
		{
			VkImageCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			createInfo.imageType = VK_IMAGE_TYPE_2D;
			createInfo.extent.width = width;
			createInfo.extent.height = height;
			createInfo.extent.depth = 1;
			createInfo.mipLevels = 1;
			createInfo.arrayLayers = 1;
			createInfo.format = format;
			createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			createInfo.usage = usage;
			createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VK_ASSERT(
				vkCreateImage(
					device,
					&createInfo,
					nullptr,
					&image
				),
				"Could not create image"
			);
		}

		return image;
	}

	void VulkanImage::UploadDataToImage(
		VkImage image,
		VkDevice device, 
		VkPhysicalDevice physicalDevice, 
		VkQueue queue, 
		uint32_t queueFamilyIndex, 
		void* data, 
		VkDeviceSize dataSize, 
		uint32_t width, 
		uint32_t height,
		VkImageLayout finalLayout,
		VkAccessFlags dstAccessMask
	)
	{
		VkBuffer stagingBuffer = VulkanBuffer::Create(
			device,
			dataSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		);

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(device, image, &memoryRequirements);

		VkDeviceMemory stagingMemory = VulkanMemory::Create(
			physicalDevice,
			device,
			memoryRequirements,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
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
		}

		VkCommandPool commandPool = VulkanCommandPool::Create(device, queueFamilyIndex);
		VkCommandBuffer commandBuffer = VulkanCommandBuffer::Create(device, commandPool);
		{
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
			}

			{
				VkImageMemoryBarrier imageBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
				imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.image = image;
				imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBarrier.subresourceRange.levelCount = 1;
				imageBarrier.subresourceRange.layerCount = 1;
				imageBarrier.srcAccessMask = 0;
				imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				vkCmdPipelineBarrier(
					commandBuffer,
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					0,
					0,
					nullptr,
					0,
					nullptr,
					1,
					&imageBarrier
				);
			}

			{
				VkBufferImageCopy region = {};
				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.layerCount = 1;
				region.imageExtent = { width, height, 1 };

				vkCmdCopyBufferToImage(
					commandBuffer, 
					stagingBuffer, 
					image, 
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
					1, 
					&region
				);
			}

			{
				VkImageMemoryBarrier imageBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
				imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageBarrier.newLayout = finalLayout;
				imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.image = image;
				imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBarrier.subresourceRange.levelCount = 1;
				imageBarrier.subresourceRange.layerCount = 1;
				imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				imageBarrier.dstAccessMask = VK_ACCESS_NONE;

				vkCmdPipelineBarrier(
					commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
					0,
					0,
					nullptr,
					0,
					nullptr,
					1,
					&imageBarrier
				);
			}

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

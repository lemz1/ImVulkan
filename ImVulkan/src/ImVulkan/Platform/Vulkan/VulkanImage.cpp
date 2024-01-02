#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/VulkanImage.h"

#include "ImVulkan/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "ImVulkan/Platform/Vulkan/Util/VulkanMemory.h"
#include "ImVulkan/Platform/Vulkan/VulkanCommandPool.h"
#include "ImVulkan/Platform/Vulkan/Buffer/VulkanCommandBuffer.h"

namespace ImVulkan
{
	VulkanImage::VulkanImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage)
	{
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
					&m_Image
				), 
				"Could not create image"
			);
		}

		{
			VkMemoryRequirements memoryRequirements;
			vkGetImageMemoryRequirements(device, m_Image, &memoryRequirements);
			VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			allocateInfo.allocationSize = memoryRequirements.size;
			allocateInfo.memoryTypeIndex = VulkanMemory::FindMemoryType(
				physicalDevice, 
				memoryRequirements.memoryTypeBits, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			VK_ASSERT(
				vkAllocateMemory(
					device, 
					&allocateInfo, 
					nullptr, 
					&m_Memory
				), 
				"Could not allocate memory!"
			);
			VK_ASSERT(
				vkBindImageMemory(
					device, 
					m_Image, 
					m_Memory, 
					0
				), 
				"Could not bind image memory!"
			);
		}

		{
			VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			createInfo.image = m_Image;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = format;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.layerCount = 1;

			VK_ASSERT(
				vkCreateImageView(
					device, 
					&createInfo, 
					nullptr, 
					&m_ImageView
				), 
				"Could not create image view!"
			);
		}
	}

	void VulkanImage::Destroy(VkDevice device)
	{
		vkDestroyImageView(device, m_ImageView, nullptr);
		vkDestroyImage(device, m_Image, nullptr);
		vkFreeMemory(device, m_Memory, nullptr);
	}

	void VulkanImage::UploadDataToImage(
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
					stagingBuffer.GetMemory(), 
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

		commandBuffer.BeginCommandBuffer();
		{
			{
				VkImageMemoryBarrier imageBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
				imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.image = m_Image;
				imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBarrier.subresourceRange.levelCount = 1;
				imageBarrier.subresourceRange.layerCount = 1;
				imageBarrier.srcAccessMask = 0;
				imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				vkCmdPipelineBarrier(
					commandBuffer.GetCommandBuffer(),
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
					commandBuffer.GetCommandBuffer(), 
					stagingBuffer.GetBuffer(), 
					m_Image, 
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
				imageBarrier.image = m_Image;
				imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBarrier.subresourceRange.levelCount = 1;
				imageBarrier.subresourceRange.layerCount = 1;
				imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				imageBarrier.dstAccessMask = VK_ACCESS_NONE;

				vkCmdPipelineBarrier(
					commandBuffer.GetCommandBuffer(),
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
		}
		commandBuffer.EndCommandBuffer();

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
}

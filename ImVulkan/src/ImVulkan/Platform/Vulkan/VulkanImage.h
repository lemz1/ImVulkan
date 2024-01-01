#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanImage
	{
	public:
		VulkanImage() = default;
		VulkanImage(
			VkDevice device, 
			VkPhysicalDevice physicalDevice, 
			uint32_t width, 
			uint32_t height, 
			VkFormat format, 
			VkImageUsageFlags usage
		);

		void Destroy(VkDevice device);

		void UploadDataToImage(
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
		);

		const VkImage& GetImage() { return m_Image; }
		const VkImageView& GetImageView() { return m_ImageView; }
		const VkDeviceMemory& GetMemory() { return m_Memory; }
	private:
		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VkDeviceMemory m_Memory = nullptr;
	};
}

#include "imvkpch.h"
#include "VulkanImageView.h"

namespace ImVulkan::VulkanImageView
{   
    VkImageView Create(
		VkDevice device,
		VkImage image,
		VkFormat format
	)
    {
		VkImageView imageView = nullptr;

		VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		createInfo.image = image;
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
				&imageView
			),
			"Could not create image view!"
		);

		return imageView;
    }
}

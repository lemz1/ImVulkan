#include "imvkpch.h"
#include "ImVulkan/Vulkan/Buffer/VulkanFrameBuffer.h"

namespace ImVulkan::VulkanFrameBuffer
{
	VkFramebuffer Create(
		VkDevice device, 
		VkRenderPass renderPass, 
		VkImageView imageView, 
		uint32_t width, 
		uint32_t height
	)
	{
		VkFramebuffer framebuffer;

		VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &imageView;
		createInfo.width = width;
		createInfo.height = height;
		createInfo.layers = 1;
		VK_ASSERT(
			vkCreateFramebuffer(
				device,
				&createInfo,
				nullptr,
				&framebuffer
			),
			"Could not create framebuffer"
		);

		return framebuffer;
	}
}

#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"

namespace ImVulkan
{
	VulkanFrameBuffer::VulkanFrameBuffer(VkDevice device, VkRenderPass renderPass, VkImageView imageView, uint32_t width, uint32_t height)
	{
		VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &imageView;
		createInfo.width = width;
		createInfo.height = height;
		createInfo.layers = 1;
		VK_ASSERT(vkCreateFramebuffer(device, &createInfo, nullptr, &m_FrameBuffer), "Could not create framebuffer");
	}

	VulkanFrameBuffer::VulkanFrameBuffer(VulkanFrameBuffer&& other) noexcept
		: m_FrameBuffer(other.m_FrameBuffer)
	{
		other.m_FrameBuffer = nullptr;
	}

	VulkanFrameBuffer& VulkanFrameBuffer::operator=(VulkanFrameBuffer&& other) noexcept
	{
		if (this != &other)
		{
			m_FrameBuffer = other.m_FrameBuffer;

			other.m_FrameBuffer = nullptr;
		}
		return *this;
	}

	void VulkanFrameBuffer::Destroy(VkDevice device)
	{
		vkDestroyFramebuffer(device, m_FrameBuffer, nullptr);
	}
}

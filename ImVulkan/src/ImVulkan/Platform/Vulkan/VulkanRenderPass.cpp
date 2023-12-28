#include "imvkpch.h"
#include "VulkanRenderPass.h"

namespace ImVulkan
{
	VulkanRenderPass::VulkanRenderPass(VulkanContext& context, VkFormat format)
	{
		VkAttachmentDescription attachmentDescription = {};
		attachmentDescription.format = format;
		attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference attachmentReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &attachmentReference;

		VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &attachmentDescription;
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subpass;

		VK_ASSERT(vkCreateRenderPass(context.GetDevice(), &createInfo, nullptr, &m_RenderPass), "Could not create render pass");
	}

	VulkanRenderPass::VulkanRenderPass(VulkanRenderPass&& other) noexcept
		: m_RenderPass(other.m_RenderPass)
	{
		other.m_RenderPass = nullptr;
	}

	VulkanRenderPass& VulkanRenderPass::operator=(VulkanRenderPass&& other) noexcept
	{
		if (this != &other)
		{
			m_RenderPass = other.m_RenderPass;

			other.m_RenderPass = nullptr;
		}

		return *this;
	}

	void VulkanRenderPass::Destroy(VulkanContext& context)
	{
		VK_ASSERT(vkDeviceWaitIdle(context.GetDevice()), "Something went wrong when waiting on device idle!");

		vkDestroyRenderPass(context.GetDevice(), m_RenderPass, nullptr);
	}
}

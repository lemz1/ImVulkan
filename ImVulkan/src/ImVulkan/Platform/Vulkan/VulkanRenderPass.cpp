#include "imvkpch.h"
#include "VulkanRenderPass.h"

namespace ImVulkan
{
	VulkanRenderPass::VulkanRenderPass(VulkanContext* context, VkFormat format)
		: m_Context(context)
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

		VK_ASSERT(vkCreateRenderPass(m_Context->GetDevice(), &createInfo, nullptr, &m_RenderPass), "Could not create render pass");
	}

	VulkanRenderPass::VulkanRenderPass(VulkanRenderPass&& other) noexcept
		: m_RenderPass(other.m_RenderPass), m_Context(other.m_Context)
	{
		other.m_Context = nullptr;
		other.m_RenderPass = nullptr;
	}

	VulkanRenderPass& VulkanRenderPass::operator=(VulkanRenderPass&& other) noexcept
	{
		if (this != &other)
		{
			m_RenderPass = other.m_RenderPass;
			m_Context = other.m_Context;

			other.m_RenderPass = nullptr;
			other.m_Context = nullptr;
		}

		return *this;
	}

	void VulkanRenderPass::Destroy()
	{
		if (m_Context == nullptr)
		{
			return;
		}

		VK_ASSERT(vkDeviceWaitIdle(m_Context->GetDevice()), "Something went wrong when waiting on device idle!");

		vkDestroyRenderPass(m_Context->GetDevice(), m_RenderPass, nullptr);
	}
}

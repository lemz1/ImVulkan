#include "imvkpch.h"
#include "VulkanCommandBuffer.h"

namespace ImVulkan
{
	VulkanCommandBuffer::VulkanCommandBuffer(VkDevice device, VkCommandPool commandPool)
	{
		VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = commandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = 1;
		
		VK_ASSERT(vkAllocateCommandBuffers(device, &allocateInfo, &m_CommandBuffer), "Could not allocate command buffer!");
	}

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept
		: m_CommandBuffer(other.m_CommandBuffer)
	{
		other.m_CommandBuffer = nullptr;
	}

	VulkanCommandBuffer& VulkanCommandBuffer::operator=(VulkanCommandBuffer&& other) noexcept
	{
		if (this != &other)
		{
			m_CommandBuffer = other.m_CommandBuffer;

			other.m_CommandBuffer = nullptr;
		}

		return *this;
	}

	void VulkanCommandBuffer::BeginCommandBuffer()
	{
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_ASSERT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo), "Could not begin command buffer!");
	}

	void VulkanCommandBuffer::EndCommandBuffer()
	{
		VK_ASSERT(vkEndCommandBuffer(m_CommandBuffer), "Could not end command buffer!");
	}

	void VulkanCommandBuffer::BeginRenderPass(VkRenderPassBeginInfo* beginInfo)
	{
		vkCmdBeginRenderPass(m_CommandBuffer, beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandBuffer::EndRenderPass()
	{
		vkCmdEndRenderPass(m_CommandBuffer);
	}

	void VulkanCommandBuffer::BindPipeline(VkPipeline pipeline, VkPipelineBindPoint pipelineBindPoint)
	{
		vkCmdBindPipeline(m_CommandBuffer, pipelineBindPoint, pipeline);
	}

	void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		vkCmdDraw(m_CommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}
}

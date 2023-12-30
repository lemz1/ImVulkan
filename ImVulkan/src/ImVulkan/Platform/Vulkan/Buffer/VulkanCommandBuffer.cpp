#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/Buffer/VulkanCommandBuffer.h"

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

	void VulkanCommandBuffer::BeginCommandBuffer() const
	{
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_ASSERT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo), "Could not begin command buffer!");
	}

	void VulkanCommandBuffer::EndCommandBuffer() const
	{
		VK_ASSERT(vkEndCommandBuffer(m_CommandBuffer), "Could not end command buffer!");
	}

	void VulkanCommandBuffer::BeginRenderPass(VkRenderPassBeginInfo* beginInfo) const
	{
		vkCmdBeginRenderPass(m_CommandBuffer, beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandBuffer::EndRenderPass() const
	{
		vkCmdEndRenderPass(m_CommandBuffer);
	}

	void VulkanCommandBuffer::BindPipeline(VkPipeline pipeline, VkPipelineBindPoint pipelineBindPoint) const
	{
		vkCmdBindPipeline(m_CommandBuffer, pipelineBindPoint, pipeline);
	}

	void VulkanCommandBuffer::SetViewports(uint32_t firstViewport, uint32_t viewportCount, VkViewport* viewports)
	{
		vkCmdSetViewport(m_CommandBuffer, firstViewport, viewportCount, viewports);
	}

	void VulkanCommandBuffer::SetScissors(uint32_t firstScissor, uint32_t scissorCount, VkRect2D* scissors)
	{
		vkCmdSetScissor(m_CommandBuffer, firstScissor, scissorCount, scissors);
	}

	void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const
	{
		vkCmdDraw(m_CommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}
}

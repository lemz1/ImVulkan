#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer() = default;
		VulkanCommandBuffer(VkDevice device, VkCommandPool commandPool);

		VulkanCommandBuffer(const VulkanCommandBuffer& other) noexcept = delete; // for now no copying

		VulkanCommandBuffer& operator=(const VulkanCommandBuffer& other) noexcept = delete;  // for now no copying

		VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept;

		VulkanCommandBuffer& operator=(VulkanCommandBuffer&& other) noexcept;

		void BeginCommandBuffer() const;
		void EndCommandBuffer() const;

		void BeginRenderPass(VkRenderPassBeginInfo* beginInfo) const;
		void EndRenderPass() const;

		void BindPipeline(VkPipeline pipeline, VkPipelineBindPoint pipelineBindPoint) const;

		void SetViewports(uint32_t firstViewport, uint32_t viewportCount, VkViewport* viewports);
		void SetScissors(uint32_t firstScissor, uint32_t scissorCount, VkRect2D* scissors);

		void BindVertexBuffers(const VkBuffer* vertexBuffers, uint32_t vertexBufferCount, uint32_t bindingCount = 1, uint32_t firstBinding = 0);
		void BindIndexBuffer(VkBuffer indexBuffer, VkIndexType indexType = VK_INDEX_TYPE_UINT32, VkDeviceSize offset = 0);

		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) const;
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, uint32_t vertexOffset = 0, uint32_t firstInstance = 0) const;

		const VkCommandBuffer& GetCommandBuffer() { return m_CommandBuffer; }
	private:
		VkCommandBuffer m_CommandBuffer = nullptr;
	};
}

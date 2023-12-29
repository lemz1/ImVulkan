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

		void BeginCommandBuffer();
		void EndCommandBuffer();

		void BeginRenderPass(VkRenderPassBeginInfo* beginInfo);
		void EndRenderPass();

		void BindPipeline(VkPipeline pipeline, VkPipelineBindPoint pipelineBindPoint);

		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);

		const VkCommandBuffer& GetCommandBuffer() { return m_CommandBuffer; }
	private:
		VkCommandBuffer m_CommandBuffer = nullptr;
	};
}

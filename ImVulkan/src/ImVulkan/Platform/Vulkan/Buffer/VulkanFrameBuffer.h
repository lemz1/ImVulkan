#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanFrameBuffer
	{
	public:
		VulkanFrameBuffer() = default;
		VulkanFrameBuffer(
			VkDevice device, 
			VkRenderPass renderPass, 
			VkImageView imageView, 
			uint32_t width, 
			uint32_t height
		);

		VulkanFrameBuffer(const VulkanFrameBuffer& other) noexcept = delete; // for now no copying

		VulkanFrameBuffer& operator=(const VulkanFrameBuffer& other) noexcept = delete;  // for now no copying

		VulkanFrameBuffer(VulkanFrameBuffer&& other) noexcept;

		VulkanFrameBuffer& operator=(VulkanFrameBuffer&& other) noexcept;

		void Destroy(VkDevice device);

		const VkFramebuffer& GetFrameBuffer() { return m_FrameBuffer; }
	private:
		VkFramebuffer m_FrameBuffer = nullptr;
	};
}

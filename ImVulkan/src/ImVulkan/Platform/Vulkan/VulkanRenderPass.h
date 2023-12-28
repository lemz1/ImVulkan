#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"
#include "ImVulkan/Platform/Vulkan/VulkanContext.h"

namespace ImVulkan
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass() = default;
		VulkanRenderPass(VulkanContext& context, VkFormat format);

		VulkanRenderPass(const VulkanRenderPass& other) noexcept = delete; // for now no copying

		VulkanRenderPass& operator=(const VulkanRenderPass& other) noexcept = delete;  // for now no copying

		VulkanRenderPass(VulkanRenderPass&& other) noexcept;

		VulkanRenderPass& operator=(VulkanRenderPass&& other) noexcept;

		void Destroy(VulkanContext& context);

		const VkRenderPass& GetRenderPass() const { return m_RenderPass; }
	private:
		VkRenderPass m_RenderPass = nullptr;
	};
}

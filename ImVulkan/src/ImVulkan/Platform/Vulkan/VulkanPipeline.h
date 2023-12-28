#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"
#include "ImVulkan/Platform/Vulkan/VulkanContext.h"

namespace ImVulkan
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline() = default;
		VulkanPipeline(VulkanContext& context, const char* vertexFilePath, const char* fragmentFilePath, VkRenderPass renderPass, uint32_t width, uint32_t height);

		VulkanPipeline(const VulkanPipeline& other) noexcept = delete; // for now no copying

		VulkanPipeline& operator=(const VulkanPipeline& other) noexcept = delete;  // for now no copying

		VulkanPipeline(VulkanPipeline&& other) noexcept;

		VulkanPipeline& operator=(VulkanPipeline&& other) noexcept;

		void Destroy(VulkanContext& context);

		const VkPipeline GetVulkanPipeline() { return m_Pipeline; }
		const VkPipelineLayout GetVulkanPipelineLayout() { return m_PipelineLayout; }
	private:
		static VkShaderModule CreateShaderModule(VulkanContext& context, const char* shaderFilePath);
	private:
		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}

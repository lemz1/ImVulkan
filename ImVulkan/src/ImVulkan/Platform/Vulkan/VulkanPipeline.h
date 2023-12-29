#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"
#include "ImVulkan/Platform/Vulkan/VulkanContext.h"

namespace ImVulkan
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline() = default;
		VulkanPipeline(VkDevice device, uint32_t shaderStageCount, VkPipelineShaderStageCreateInfo* shaderStages, VkRenderPass renderPass);

		VulkanPipeline(const VulkanPipeline& other) noexcept = delete; // for now no copying

		VulkanPipeline& operator=(const VulkanPipeline& other) noexcept = delete;  // for now no copying

		VulkanPipeline(VulkanPipeline&& other) noexcept;

		VulkanPipeline& operator=(VulkanPipeline&& other) noexcept;

		void Destroy(VkDevice device);

		const VkPipeline GetVulkanPipeline() { return m_Pipeline; }
		const VkPipelineLayout GetVulkanPipelineLayout() { return m_PipelineLayout; }
	public:
		static VkShaderModule CreateShaderModule(VkDevice device, const char* shaderFilePath);
	private:
		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}

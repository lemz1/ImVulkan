#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanInstance
	{
	public:
		VulkanInstance() = default;
		VulkanInstance(
			uint32_t instanceExtensionCount, 
			const char** instanceExtensions
		);

		VulkanInstance(const VulkanInstance& other) noexcept = delete; // for now no copying

		VulkanInstance& operator=(const VulkanInstance& other) noexcept = delete;  // for now no copying

		VulkanInstance(VulkanInstance&& other) noexcept;

		VulkanInstance& operator=(VulkanInstance&& other) noexcept;

		void Destroy();

		const VkInstance& GetInstance() const { return m_Instance; }
	private:
		VkInstance m_Instance = nullptr;
	};
}

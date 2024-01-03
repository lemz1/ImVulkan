#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace ImVulkan
{
	class GLFWImGuiContext
	{
	public:
		GLFWImGuiContext() = default;
		GLFWImGuiContext(
			GLFWwindow* windowHandle,
			VkInstance instance,
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			VkQueue queue,
			size_t imageCount,
			VkRenderPass renderPass
		);

		void Destroy(VkDevice device);
	private:
		VkDescriptorPool m_DescriptorPool = nullptr;
	};
}

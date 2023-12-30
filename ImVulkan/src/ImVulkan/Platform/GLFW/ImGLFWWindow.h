#pragma once

#include "ImVulkan/Core/Window.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ImVulkan/Platform/Vulkan/VulkanContext.h"
#include "ImVulkan/Platform/Vulkan/VulkanSwapchain.h"
#include "ImVulkan/Platform/Vulkan/VulkanRenderPass.h"
#include "ImVulkan/Platform/Vulkan/VulkanPipeline.h"
#include "ImVulkan/Platform/Vulkan/VulkanSemaphore.h"
#include "ImVulkan/Platform/Vulkan/VulkanFence.h"
#include "ImVulkan/Platform/Vulkan/VulkanCommandPool.h"
#include "ImVulkan/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "ImVulkan/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"
#include "ImVulkan/Platform/Vulkan/Buffer/VulkanCommandBuffer.h"

#define FRAMES_IN_FLIGHT 2

namespace ImVulkan
{
	class ImGLFWWindow : public Window
	{
	public:
		ImGLFWWindow(const WindowSpecification& spec);
		~ImGLFWWindow() override;

		void* GetNativeWindow() override;

		const char* GetTitle() override;
		void SetTitle(const char* title) override;

		const uint32_t GetWidth() override;
		const uint32_t GetHeight() override;

		void Resize(uint32_t width, uint32_t height) override;

		const bool GetVSync() const override;
		void SetVSync(bool vSync) override;

		void PollEvents() override;

		const bool ShouldClose() const override;

		void OnUpdate() override;
	private:
		void RecreateSwapchain();
	private:
		static void ErrorCallback(int error, const char* description);
	private:
		WindowSpecification m_Spec;

		GLFWwindow* m_WindowHandle;
		VulkanContext m_VulkanContext;

		VkSurfaceKHR m_Surface;
		VulkanSwapchain m_Swapchain;
		VulkanRenderPass m_RenderPass;
		std::vector<VulkanFrameBuffer> m_FrameBuffers;
		VulkanPipeline m_VulkanPipeline;
		VulkanCommandBuffer m_CommandBuffers[FRAMES_IN_FLIGHT];
		VulkanCommandPool m_CommandPools[FRAMES_IN_FLIGHT];
		VulkanFence m_Fences[FRAMES_IN_FLIGHT];
		VulkanSemaphore m_AcquireSephamores[FRAMES_IN_FLIGHT];
		VulkanSemaphore m_ReleaseSephamores[FRAMES_IN_FLIGHT];
		VulkanBuffer m_VertexBuffer;
		VulkanBuffer m_IndexBuffer;

		bool m_Minimized = false;
	};
}

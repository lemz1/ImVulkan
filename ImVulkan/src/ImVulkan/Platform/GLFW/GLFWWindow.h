#pragma once
#ifdef IMVK_PLATFORM_GLFW

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
#include "ImVulkan/Platform/Vulkan/VulkanImage.h"
#include "ImVulkan/Platform/GLFW/GLFWImGuiContext.h"

namespace ImVulkan
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowSpecification& spec);
		~GLFWWindow() override;

		void* GetNativeWindow() override;

		const char* GetTitle() override { return m_Data.title; }
		void SetTitle(const char* title) override { m_Data.title = title; glfwSetWindowTitle(m_WindowHandle, title); }

		const uint32_t GetWidth() override { return m_Data.width; }
		const uint32_t GetHeight() override { return m_Data.height; }

		void Resize(
			uint32_t width, 
			uint32_t height
		) override;

		const bool GetVSync() const override { return m_Data.vSync; }
		void SetVSync(bool vSync) override { m_Data.vSync = vSync; RecreateSwapchain(); }

		void PollEvents() override;

		const bool ShouldClose() const override;

		void OnUpdate() override;

		void SetEventCallback(const EventCallback& callback) { m_Data.eventCallback = callback; }

		const VulkanContext& GetVulkanContext() { return m_VulkanContext; }
		const VkSurfaceKHR& GetSurface() { return m_Surface; }
		const VulkanSwapchain& GetSwapchain() { return m_Swapchain; }
		const VulkanRenderPass& GetRenderPass() { return m_RenderPass; }
		const std::vector<VulkanFrameBuffer>& GetFrameBuffers() { return m_FrameBuffers; }
		const VulkanPipeline& GetPipeline() { return m_VulkanPipeline; }
	private:
		void InitEventCallbacks();

		void RecreateSwapchain();
	private:
		static void ErrorCallback(
			int error, 
			const char* description
		);
	private:
		GLFWwindow* m_WindowHandle;
		VulkanContext m_VulkanContext;

		VkSurfaceKHR m_Surface;
		VulkanSwapchain m_Swapchain;
		VulkanRenderPass m_RenderPass;
		std::vector<VulkanFrameBuffer> m_FrameBuffers;
		VulkanPipeline m_VulkanPipeline;
		VulkanCommandBuffer m_CommandBuffer;
		VulkanCommandPool m_CommandPool;
		VulkanFence m_Fence;
		VulkanSemaphore m_AcquireSephamore;
		VulkanSemaphore m_ReleaseSephamore;
		VulkanBuffer m_VertexBuffer;
		VulkanBuffer m_IndexBuffer;
		VulkanImage m_Image;

		VkSampler m_Sampler;
		VkDescriptorSet m_Descriptor;
		VkDescriptorPool m_DescriptorPool;
		VkDescriptorSetLayout m_DescriptorSetLayout;

		struct WindowData
		{
			const char* title;
			uint32_t width, height;
			bool vSync;
			bool minimized;
			EventCallback eventCallback;
		};

		WindowData m_Data;

		GLFWImGuiContext m_ImGuiContext;
	};
}
#endif

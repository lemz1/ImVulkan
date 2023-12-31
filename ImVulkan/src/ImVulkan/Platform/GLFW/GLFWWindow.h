#pragma once
#ifdef IMVK_PLATFORM_GLFW

#include "ImVulkan/Core/Window.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ImVulkan/Vulkan/VulkanContext.h"
#include "ImVulkan/Vulkan/VulkanSwapchain.h"
#include "ImVulkan/Vulkan/VulkanRenderPass.h"
#include "ImVulkan/Vulkan/VulkanPipeline.h"
#include "ImVulkan/Vulkan/VulkanSemaphore.h"
#include "ImVulkan/Vulkan/VulkanFence.h"
#include "ImVulkan/Vulkan/VulkanCommandPool.h"
#include "ImVulkan/Vulkan/Buffer/VulkanBuffer.h"
#include "ImVulkan/Vulkan/Buffer/VulkanFrameBuffer.h"
#include "ImVulkan/Vulkan/Buffer/VulkanCommandBuffer.h"
#include "ImVulkan/Vulkan/VulkanImage.h"
#include "ImVulkan/Platform/GLFW/GLFWImGuiContext.h"

namespace ImVulkan
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowSpecification& spec);

		void Destroy(
			VkInstance instance,
			VkDevice device
		) override;

		void InitVulkan(
			VkInstance instance,
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			VkQueue queue
		) override;

		void GetInstanceExtensions(
			uint32_t& outInstanceExtensionCount,
			const char**& outInstanceExtensions
		) override;

		void GetDeviceExtensions(
			uint32_t& outDeviceExtensionCount,
			const char**& outDeviceExtensions
		) override;

		void* GetNativeWindow() override { return m_WindowHandle; }

		const char* GetTitle() const override { return m_Data.title; }
		void SetTitle(const char* title) override { m_Data.title = title; glfwSetWindowTitle(m_WindowHandle, title); }

		const uint32_t GetWidth() const override { return m_Data.width; }
		const uint32_t GetHeight() const override { return m_Data.height; }

		void Resize(
			uint32_t width, 
			uint32_t height,
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex
		) override;

		const bool GetVSync() const override { return m_Data.vSync; }
		void SetVSync(
			bool vSync,
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex
		) override { m_Data.vSync = vSync; RecreateSwapchain(physicalDevice, device, queueFamilyIndex); }

		void PollEvents() override;

		const bool ShouldClose() const override;

		const bool AcquireNextImage(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			VkSemaphore signalSemaphore
		) override;

		void BeginImGuiFrame() override;
		ImDrawData* EndImGuiFrame() override;

		void SwapBuffers(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			VkQueue queue,
			VkFence fence,
			VkSemaphore waitSemaphore
		) override;

		void RecreateSwapchain(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex
		) override;

		void SetEventCallback(const EventCallback& callback) { m_Data.eventCallback = callback; }
		void SetRecreateSwapchain(const PFN_RecreateSwapchain& recreateSwapchain) { m_Data.recreateSwapchain = recreateSwapchain; }

		const VkSurfaceKHR& GetSurface() const override { return m_Surface; }
		const VkSwapchainKHR& GetSwapchain() const override { return m_Swapchain.swapchain; }
		const uint32_t& GetCurrentImageIndex() const override { return m_ImageIndex; }
		const VkImage& GetCurrentImage() const override { return m_Swapchain.images[m_ImageIndex]; }
		const VkImageLayout& GetCurrentImageLayout() const override { return m_Swapchain.imageLayouts[m_ImageIndex]; }
		const VkImageView& GetCurrentImageView() const override { return m_Swapchain.imageViews[m_ImageIndex]; }

		const VkRenderPass& GetRenderPass() const override { return m_RenderPass; }
		const VkFramebuffer& GetCurrentFrameBuffer() const override { return m_FrameBuffers[m_ImageIndex]; }
	private:
		void InitEventCallbacks();

	private:
		static void ErrorCallback(
			int error, 
			const char* description
		);
	private:
		static uint32_t instanceExtensionCount;
		static const char** instanceExtensions;
		static uint32_t deviceExtensionCount;
		static const char** deviceExtensions;
	private:
		GLFWwindow* m_WindowHandle;

		VkSurfaceKHR m_Surface;
		VulkanSwapchain m_Swapchain;
		uint32_t m_ImageIndex = 0;
		VkRenderPass m_RenderPass;
		std::vector<VkFramebuffer> m_FrameBuffers;

		struct WindowData
		{
			const char* title;
			uint32_t width, height;
			bool vSync;
			bool minimized;
			EventCallback eventCallback;
			PFN_RecreateSwapchain recreateSwapchain;
		};

		WindowData m_Data;

		GLFWImGuiContext m_ImGuiContext;
	};
}
#endif

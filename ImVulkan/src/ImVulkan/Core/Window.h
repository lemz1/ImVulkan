#pragma once

#include "ImVulkan/Event/Event.h"

#include <stdint.h>
#include <imgui.h>
#include <vulkan/vulkan.h>

namespace ImVulkan
{
	using EventCallback = std::function<void(Event&)>;

	struct WindowSpecification
	{
		const char* title = "ImVulkan Application";
		uint32_t width = 1280, height = 720;
		bool vSync = true;
	};

	class Window
	{
	public:
		static Window* Create(const WindowSpecification& spec);
	public:
		virtual ~Window() = default;

		virtual void Destroy(
			VkInstance instance,
			VkDevice device
		) = 0;

		virtual void InitVulkan(
			VkInstance instance,
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			VkQueue queue
		) = 0;

		virtual void GetInstanceExtensions(
			uint32_t& outInstanceExtensionCount,
			const char**& outInstanceExtensions
		) = 0;

		virtual void GetDeviceExtensions(
			uint32_t& outDeviceExtensionCount,
			const char**& outDeviceExtensions
		) = 0;

		// pointer to HWND, GLFWwindow, etc.
		virtual void* GetNativeWindow() = 0;

		virtual const char* GetTitle() = 0;
		virtual void SetTitle(const char* title) = 0;

		virtual const uint32_t GetWidth() = 0;
		virtual const uint32_t GetHeight() = 0;

		virtual void Resize(
			uint32_t width,
			uint32_t height,
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex
		) = 0;

		virtual const bool GetVSync() const = 0;
		virtual void SetVSync(
			bool vSync,
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex
		) = 0;

		virtual void PollEvents() = 0;

		virtual const bool ShouldClose() const = 0;

		virtual const bool AcquireNextImage(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex
		) = 0;

		virtual void BeginImGuiFrame() = 0;
		virtual ImDrawData* EndImGuiFrame() = 0;

		// swap buffers, etc.
		virtual void SwapBuffers(
			ImDrawData* imGuiDrawData,
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			VkQueue queue,
			VkFence fence
		) = 0;

		virtual void SetEventCallback(const EventCallback& callback) = 0;

		virtual const VkSurfaceKHR& GetSurface() const = 0;
		virtual const VkSwapchainKHR& GetSwapchain() const = 0;
		virtual const VkImage& GetCurrentImage() const = 0;
		virtual const VkImageView& GetCurrentImageView() const = 0;

		virtual const VkRenderPass& GetRenderPass() const = 0;
		virtual const VkFramebuffer& GetCurrentFrameBuffer() const = 0;
	};
}

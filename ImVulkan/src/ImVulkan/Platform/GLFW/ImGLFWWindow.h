#pragma once

#include "ImVulkan/Core/Window.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ImVulkan/Platform/Vulkan/VulkanContext.h"

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
		static void ErrorCallback(int error, const char* description);
	private:
		WindowSpecification m_Spec;

		GLFWwindow* m_WindowHandle;
		VulkanContext m_VulkanContext;
	};
}

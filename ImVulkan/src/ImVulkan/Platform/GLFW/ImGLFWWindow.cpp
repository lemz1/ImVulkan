#include "imvkpch.h"
#include "ImVulkan/Platform/GLFW/ImGLFWWindow.h"

#include "ImVulkan/Core/Core.h"

namespace ImVulkan
{
	Window* Window::Create(const WindowSpecification& spec)
	{
		return new ImGLFWWindow(spec);
	}

	ImGLFWWindow::ImGLFWWindow(const WindowSpecification& spec)
	{
		glfwSetErrorCallback(ErrorCallback);
		IMVK_ASSERT(!glfwInit(), "Could not initialize GLFW!");

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_WindowHandle = glfwCreateWindow(spec.width, spec.height, spec.title, nullptr, nullptr);

		IMVK_ASSERT(!glfwVulkanSupported(), "GLFW: Vulkan is not supported!");

		uint32_t extensionCount = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

		m_VulkanContext.Setup(extensionCount, extensions);

		glfwMakeContextCurrent(m_WindowHandle);
	}

	ImGLFWWindow::~ImGLFWWindow()
	{
		glfwDestroyWindow(m_WindowHandle);
	}

	void* ImGLFWWindow::GetNativeWindow()
	{
		return m_WindowHandle;
	}

	const char* ImGLFWWindow::GetTitle()
	{
		return m_Spec.title;
	}

	void ImGLFWWindow::SetTitle(const char* title)
	{
		m_Spec.title = title;
	}

	const uint32_t ImGLFWWindow::GetWidth()
	{
		return m_Spec.width;
	}

	const uint32_t ImGLFWWindow::GetHeight()
	{
		return m_Spec.height;
	}

	void ImGLFWWindow::Resize(uint32_t width, uint32_t height)
	{
		if (width == m_Spec.width && height == m_Spec.height)
		{
			return;
		}

		m_Spec.width = width;
		m_Spec.height = height;
	}

	const bool ImGLFWWindow::GetVSync() const
	{
		return m_Spec.vSync;
	}

	void ImGLFWWindow::SetVSync(bool vSync)
	{
		m_Spec.vSync = vSync;
	}

	void ImGLFWWindow::PollEvents()
	{
		glfwPollEvents();
	}

	const bool ImGLFWWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_WindowHandle);
	}

	void ImGLFWWindow::OnUpdate()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void ImGLFWWindow::ErrorCallback(int error, const char* description)
	{
		fprintf(stderr, "GLFW Error %d: %s\n", error, description);
	}
}

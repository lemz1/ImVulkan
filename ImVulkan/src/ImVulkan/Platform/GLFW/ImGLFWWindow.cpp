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

		uint32_t instanceExtensionCount = 0;
		const char** instanceExtensions = glfwGetRequiredInstanceExtensions(&instanceExtensionCount);

		const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		m_VulkanContext.CreateDevice(instanceExtensionCount, instanceExtensions, ARRAY_COUNT(deviceExtensions), deviceExtensions);

		VkSurfaceKHR surface; 
		glfwCreateWindowSurface(m_VulkanContext.GetInstance(), m_WindowHandle, nullptr, &surface);
		
		m_Swapchain = VulkanSwapchain(&m_VulkanContext, surface, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		m_RenderPass = VulkanRenderPass(&m_VulkanContext, m_Swapchain.GetFormat());
		m_FrameBuffers.resize(m_Swapchain.GetImages().size());
		for (uint32_t i = 0; i < m_Swapchain.GetImages().size(); i++)
		{
			VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			createInfo.renderPass = m_RenderPass.GetRenderPass();
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = &m_Swapchain.GetImageViews()[i];
			createInfo.width = m_Swapchain.GetWidth();
			createInfo.height = m_Swapchain.GetHeight();
			createInfo.layers = 1;
			vkCreateFramebuffer(m_VulkanContext.GetDevice(), &createInfo, nullptr, &m_FrameBuffers[i]);
		}

		glfwMakeContextCurrent(m_WindowHandle);
	}

	ImGLFWWindow::~ImGLFWWindow()
	{
		m_RenderPass.Destroy();
		m_Swapchain.Destroy();

		for (uint32_t i = 0; i < m_FrameBuffers.size(); i++)
		{
			vkDestroyFramebuffer(m_VulkanContext.GetDevice(), m_FrameBuffers[i], nullptr);
		}

		m_VulkanContext.Destroy();

		glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();
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

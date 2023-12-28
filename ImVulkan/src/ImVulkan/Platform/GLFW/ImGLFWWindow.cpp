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
		
		m_Swapchain = VulkanSwapchain(m_VulkanContext, surface, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		m_RenderPass = VulkanRenderPass(m_VulkanContext, m_Swapchain.GetFormat());
		m_VulkanPipeline = VulkanPipeline(m_VulkanContext, "assets/shaders/triangle.vert.spv", "assets/shaders/triangle.frag.spv",
										  m_RenderPass.GetRenderPass(), m_Swapchain.GetWidth(), m_Swapchain.GetHeight());

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

		{
			VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			VK_ASSERT(vkCreateFence(m_VulkanContext.GetDevice(), &createInfo, nullptr, &m_Fence), "Could not create fence!");
		}

		{
			VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
			createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			createInfo.queueFamilyIndex = m_VulkanContext.GetGraphicsQueue().queueIndex;
			VK_ASSERT(vkCreateCommandPool(m_VulkanContext.GetDevice(), &createInfo, nullptr, &m_CommandPool), "Could not create command pool!");
		}

		{
			VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			allocateInfo.commandPool = m_CommandPool;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = 1;

			VK_ASSERT(vkAllocateCommandBuffers(m_VulkanContext.GetDevice(), &allocateInfo, &m_CommandBuffer), "Could not allocate command buffer!");
		}

		glfwMakeContextCurrent(m_WindowHandle);
	}

	ImGLFWWindow::~ImGLFWWindow()
	{
		vkDestroyFence(m_VulkanContext.GetDevice(), m_Fence, nullptr);
		vkDestroyCommandPool(m_VulkanContext.GetDevice(), m_CommandPool, nullptr);

		m_VulkanPipeline.Destroy(m_VulkanContext);
		m_RenderPass.Destroy(m_VulkanContext);
		m_Swapchain.Destroy(m_VulkanContext);

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
		uint32_t imageIndex;
		VK_ASSERT(vkAcquireNextImageKHR(m_VulkanContext.GetDevice(), m_Swapchain.GetSwapchain(), UINT64_MAX, 0, m_Fence, &imageIndex), "Could not acquire next image");

		VK_ASSERT(vkResetCommandPool(m_VulkanContext.GetDevice(), m_CommandPool, 0), "Could not reset command pool!");

		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_ASSERT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo), "Could not begin command buffer!");
		{
			VkClearValue clearValue = {.1f, .1f, .1f, 1.f};

			VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			beginInfo.renderPass = m_RenderPass.GetRenderPass();
			beginInfo.framebuffer = m_FrameBuffers[imageIndex];
			beginInfo.renderArea = { {0, 0}, {m_Swapchain.GetWidth(), m_Swapchain.GetHeight()} };
			beginInfo.clearValueCount = 1;
			beginInfo.pClearValues = &clearValue;
			vkCmdBeginRenderPass(m_CommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetVulkanPipeline());
			vkCmdDraw(m_CommandBuffer, 3, 1, 0, 0);

			vkCmdEndRenderPass(m_CommandBuffer);
		}
		VK_ASSERT(vkEndCommandBuffer(m_CommandBuffer), "Could not end command buffer!");

		VK_ASSERT(vkWaitForFences(m_VulkanContext.GetDevice(), 1, &m_Fence, VK_TRUE, UINT64_MAX), "Could not wait for fences!");
		VK_ASSERT(vkResetFences(m_VulkanContext.GetDevice(), 1, &m_Fence), "Could not reset fences!");

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;
		VK_ASSERT(vkQueueSubmit(m_VulkanContext.GetGraphicsQueue().queue, 1, &submitInfo, 0), "Could not submit queue");

		VK_ASSERT(vkDeviceWaitIdle(m_VulkanContext.GetDevice()), "Could not wait for device idle!");

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain.GetSwapchain();
		presentInfo.pImageIndices = &imageIndex;
		vkQueuePresentKHR(m_VulkanContext.GetGraphicsQueue().queue, &presentInfo);

		glfwSwapBuffers(m_WindowHandle);
	}

	void ImGLFWWindow::ErrorCallback(int error, const char* description)
	{
		fprintf(stderr, "GLFW Error %d: %s\n", error, description);
	}
}

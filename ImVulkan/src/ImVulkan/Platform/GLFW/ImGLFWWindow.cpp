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

		{
			uint32_t instanceExtensionCount = 0;
			const char** instanceExtensions = glfwGetRequiredInstanceExtensions(&instanceExtensionCount);

			const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

			m_VulkanContext.CreateDevice(instanceExtensionCount, instanceExtensions, ARRAY_COUNT(deviceExtensions), deviceExtensions);
		}

		{
			VkSurfaceKHR surface;
			glfwCreateWindowSurface(m_VulkanContext.GetInstance(), m_WindowHandle, nullptr, &surface);

			m_Swapchain = VulkanSwapchain(m_VulkanContext, surface, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		}

		{
			m_RenderPass = VulkanRenderPass(m_VulkanContext, m_Swapchain.GetFormat());
		}

		{
			VkShaderModule vertexModule = VulkanPipeline::CreateShaderModule(m_VulkanContext, "assets/shaders/triangle.vert.spv");
			VkShaderModule fragmentModule = VulkanPipeline::CreateShaderModule(m_VulkanContext, "assets/shaders/triangle.frag.spv");

			VkPipelineShaderStageCreateInfo shaderStages[2];
			shaderStages[0] = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
			shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
			shaderStages[0].module = vertexModule;
			shaderStages[0].pName = "main";

			shaderStages[1] = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
			shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			shaderStages[1].module = fragmentModule;
			shaderStages[1].pName = "main";

			m_VulkanPipeline = VulkanPipeline(m_VulkanContext, ARRAY_COUNT(shaderStages), shaderStages,
											  m_RenderPass.GetRenderPass(), m_Swapchain.GetWidth(), m_Swapchain.GetHeight());
		}

		m_FrameBuffers.resize(m_Swapchain.GetImages().size());
		for (uint32_t i = 0; i < m_Swapchain.GetImages().size(); i++)
		{
			m_FrameBuffers[i] = VulkanFrameBuffer(
				m_VulkanContext.GetDevice(), 
				m_RenderPass.GetRenderPass(), 
				m_Swapchain.GetImageViews()[i], 
				m_Swapchain.GetWidth(), 
				m_Swapchain.GetHeight()
			);
		}

		for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_Fences[i] = VulkanFence(m_VulkanContext.GetDevice());

			m_AcquireSephamores[i] = VulkanSemaphore(m_VulkanContext.GetDevice());
			m_ReleaseSephamores[i] = VulkanSemaphore(m_VulkanContext.GetDevice());

			m_CommandPools[i] = VulkanCommandPool(m_VulkanContext.GetDevice(), m_VulkanContext.GetGraphicsQueue().queueFamilyIndex);

			m_CommandBuffers[i] = VulkanCommandBuffer(m_VulkanContext.GetDevice(), m_CommandPools[i].GetCommandPool());
		}
	}

	ImGLFWWindow::~ImGLFWWindow()
	{
		VK_ASSERT(vkDeviceWaitIdle(m_VulkanContext.GetDevice()), "Something went wrong when waiting on device idle!");

		for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_AcquireSephamores[i].Destroy(m_VulkanContext.GetDevice());
			m_ReleaseSephamores[i].Destroy(m_VulkanContext.GetDevice());
			m_CommandPools[i].Destroy(m_VulkanContext.GetDevice());
			m_Fences[i].Destroy(m_VulkanContext.GetDevice());
		}

		m_VulkanPipeline.Destroy(m_VulkanContext);
		m_RenderPass.Destroy(m_VulkanContext);
		m_Swapchain.Destroy(m_VulkanContext);

		for (uint32_t i = 0; i < m_FrameBuffers.size(); i++)
		{
			m_FrameBuffers[i].Destroy(m_VulkanContext.GetDevice());
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
		static uint32_t frameIndex;

		m_Fences[frameIndex].Wait(m_VulkanContext.GetDevice());
		m_Fences[frameIndex].Reset(m_VulkanContext.GetDevice());

		uint32_t imageIndex;
		VK_ASSERT(
			vkAcquireNextImageKHR(
				m_VulkanContext.GetDevice(), 
				m_Swapchain.GetSwapchain(), 
				UINT64_MAX, m_AcquireSephamores[frameIndex].GetSemaphore(),
				nullptr, 
				&imageIndex
			), 
			"Could not acquire next image");

		m_CommandPools[frameIndex].Reset(m_VulkanContext.GetDevice());

		{
			VulkanCommandBuffer& commandBuffer = m_CommandBuffers[frameIndex];

			// CommandBuffer Info
			{
				commandBuffer.BeginCommandBuffer();
			}

			// Renderpass Info
			{
				VkClearValue clearValue = { .1f, .1f, .1f, 1.f };
				VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
				beginInfo.renderPass = m_RenderPass.GetRenderPass();
				beginInfo.framebuffer = m_FrameBuffers[imageIndex].GetFrameBuffer();
				beginInfo.renderArea = { {0, 0}, {m_Swapchain.GetWidth(), m_Swapchain.GetHeight()} };
				beginInfo.clearValueCount = 1;
				beginInfo.pClearValues = &clearValue;

				commandBuffer.BeginRenderPass(&beginInfo);
			}

			// Binding Pipeline
			{
				commandBuffer.BindPipeline(m_VulkanPipeline.GetVulkanPipeline(), VK_PIPELINE_BIND_POINT_GRAPHICS);
			}

			// Actual Rendering
			{
				commandBuffer.Draw(3);
			}

			// End RenderPass
			{
				commandBuffer.EndRenderPass();
			}

			// End CommandBuffer
			{
				commandBuffer.EndCommandBuffer();
			}
		}


		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[frameIndex].GetCommandBuffer();
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_AcquireSephamores[frameIndex].GetSemaphore();
		VkPipelineStageFlags waitMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		submitInfo.pWaitDstStageMask = &waitMask;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_ReleaseSephamores[frameIndex].GetSemaphore();
		VK_ASSERT(vkQueueSubmit(m_VulkanContext.GetGraphicsQueue().queue, 1, &submitInfo, m_Fences[frameIndex].GetFence()), "Could not submit queue");

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain.GetSwapchain();
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_ReleaseSephamores[frameIndex].GetSemaphore();
		vkQueuePresentKHR(m_VulkanContext.GetGraphicsQueue().queue, &presentInfo);

		frameIndex = (frameIndex + 1) % FRAMES_IN_FLIGHT;
	}

	void ImGLFWWindow::ErrorCallback(int error, const char* description)
	{
		fprintf(stderr, "GLFW Error %d: %s\n", error, description);
	}
}

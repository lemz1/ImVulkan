#include "imvkpch.h"
#ifdef IMVK_PLATFORM_GLFW
#include "ImVulkan/Platform/GLFW/GLFWWindow.h"

#include "ImVulkan/Core/Core.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "ImVulkan/Event/KeyboardEvent.h"
#include "ImVulkan/Event/MouseEvent.h"
#include "ImVulkan/Event/WindowEvent.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"

namespace ImVulkan
{
	Window* Window::Create(const WindowSpecification& spec)
	{
		return new GLFWWindow(spec);
	}

	GLFWWindow::GLFWWindow(const WindowSpecification& spec)
	{
		m_Data.title = spec.title;
		m_Data.width = spec.width;
		m_Data.height = spec.height;
		m_Data.vSync = spec.vSync;
		m_Data.minimized = false;
		m_Data.eventCallback = nullptr;

		glfwSetErrorCallback(ErrorCallback);
		IMVK_ASSERT(!glfwInit(), "Could not initialize GLFW!");

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_WindowHandle = glfwCreateWindow(spec.width, spec.height, spec.title, nullptr, nullptr);

		InitEventCallbacks();

		IMVK_ASSERT(!glfwVulkanSupported(), "GLFW: Vulkan is not supported!");

		{
			const char* additionalInstanceExtensions[] = {
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
				VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME
			};

			uint32_t instanceExtensionCount = 0;
			const char** instanceExtensions = glfwGetRequiredInstanceExtensions(&instanceExtensionCount);

			const char** mergedInstanceExtensions = new const char* [instanceExtensionCount + ARRAY_COUNT(additionalInstanceExtensions)];

			memcpy(mergedInstanceExtensions, instanceExtensions, instanceExtensionCount * sizeof(const char*));

			memcpy(mergedInstanceExtensions + instanceExtensionCount, additionalInstanceExtensions, ARRAY_COUNT(additionalInstanceExtensions) * sizeof(const char*));

			const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

			m_VulkanContext = VulkanContext(
				instanceExtensionCount + ARRAY_COUNT(additionalInstanceExtensions), mergedInstanceExtensions, 
				ARRAY_COUNT(deviceExtensions), deviceExtensions
			);

			m_VulkanContext.InitDebugMessenger();

			delete[] mergedInstanceExtensions;
		}

		{
			glfwCreateWindowSurface(m_VulkanContext.GetInstance(), m_WindowHandle, nullptr, &m_Surface);

			m_Swapchain = VulkanSwapchain(
				m_VulkanContext.GetDevice(),
				m_VulkanContext.GetPhysicalDevice(),
				m_VulkanContext.GetQueueFamilyIndex(),
				m_Surface,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				m_Data.vSync
			);
		}

		{
			m_RenderPass = VulkanRenderPass(m_VulkanContext.GetDevice(), m_Swapchain.GetFormat());
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

		{
			m_Fence = VulkanFence(m_VulkanContext.GetDevice());
		}

		{
			m_AcquireSephamore = VulkanSemaphore(m_VulkanContext.GetDevice());
			m_ReleaseSephamore = VulkanSemaphore(m_VulkanContext.GetDevice());
		}

		{
			m_CommandPool = VulkanCommandPool(m_VulkanContext.GetDevice(), m_VulkanContext.GetQueueFamilyIndex());
		}

		{
			m_CommandBuffer = VulkanCommandBuffer(m_VulkanContext.GetDevice(), m_CommandPool.GetCommandPool());
		}

		m_ImGuiContext = GLFWImGuiContext(
			m_WindowHandle,
			m_VulkanContext.GetInstance(),
			m_VulkanContext.GetPhysicalDevice(),
			m_VulkanContext.GetDevice(),
			m_VulkanContext.GetQueueFamilyIndex(),
			m_VulkanContext.GetQueue(),
			m_Swapchain.GetImages().size(),
			m_RenderPass.GetRenderPass()
		);
	}

	GLFWWindow::~GLFWWindow()
	{
		VK_ASSERT(vkDeviceWaitIdle(m_VulkanContext.GetDevice()), "Something went wrong when waiting on device idle!");

		{
			m_ImGuiContext.Destroy(m_VulkanContext.GetDevice());
		}

		{
			m_AcquireSephamore.Destroy(m_VulkanContext.GetDevice());
			m_ReleaseSephamore.Destroy(m_VulkanContext.GetDevice());
			m_CommandPool.Destroy(m_VulkanContext.GetDevice());
			m_Fence.Destroy(m_VulkanContext.GetDevice());
		}

		{
			m_RenderPass.Destroy(m_VulkanContext.GetDevice());
			m_Swapchain.Destroy(m_VulkanContext.GetDevice());
			vkDestroySurfaceKHR(m_VulkanContext.GetInstance(), m_Surface, nullptr);
		}


		{
			for (uint32_t i = 0; i < m_FrameBuffers.size(); i++)
			{
				m_FrameBuffers[i].Destroy(m_VulkanContext.GetDevice());
			}
			m_FrameBuffers.clear();
		}

		{
			m_VulkanContext.Destroy();
		}

		{
			glfwDestroyWindow(m_WindowHandle);
			glfwTerminate();
		}
	}

	void* GLFWWindow::GetNativeWindow()
	{
		return m_WindowHandle;
	}

	void GLFWWindow::Resize(uint32_t width, uint32_t height)
	{
		if (m_Data.width == width && m_Data.height == height)
		{
			return;
		}

		m_Data.width = width;
		m_Data.height = height;

		glfwSetWindowSize(m_WindowHandle, width, height);

		RecreateSwapchain();
	}

	void GLFWWindow::PollEvents()
	{
		glfwPollEvents();
	}

	const bool GLFWWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_WindowHandle);
	}

	void GLFWWindow::OnUpdate()
	{
		if (m_Data.minimized)
		{
			return;
		}

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::ShowDemoWindow();

		ImGui::Render();
		ImDrawData* drawData = ImGui::GetDrawData();

		m_Fence.Wait(m_VulkanContext.GetDevice());

		uint32_t imageIndex;
		{
			// if acquire next image gets called when the window is minimized it will lead to a validation error
			// the only way to not make that happen is to return early when the window is minimized

			VkResult result = vkAcquireNextImageKHR(
				m_VulkanContext.GetDevice(),
				m_Swapchain.GetSwapchain(),
				UINT64_MAX, m_AcquireSephamore.GetSemaphore(),
				nullptr,
				&imageIndex
			);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				RecreateSwapchain();
				return;
			}
			else
			{
				m_Fence.Reset(m_VulkanContext.GetDevice());
				VK_ASSERT(result, "Could not acquire next image")
			}
		}

		m_CommandPool.Reset(m_VulkanContext.GetDevice());

		m_CommandBuffer.BeginCommandBuffer();
		{
			{ // Window Resize
				VkViewport viewport = { 0.f, 0.f, (float)m_Swapchain.GetWidth(), (float)m_Swapchain.GetHeight() };
				VkRect2D scissor = { {0, 0}, {m_Swapchain.GetWidth(), m_Swapchain.GetHeight()} };
				m_CommandBuffer.SetViewports(0, 1, &viewport);
				m_CommandBuffer.SetScissors(0, 1, &scissor);
			}

			{ // Begin RenderPass
				VkClearValue clearValue = { .1f, .1f, .1f, 1.f };
				VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
				beginInfo.renderPass = m_RenderPass.GetRenderPass();
				beginInfo.framebuffer = m_FrameBuffers[imageIndex].GetFrameBuffer();
				beginInfo.renderArea = { {0, 0}, {m_Swapchain.GetWidth(), m_Swapchain.GetHeight()} };
				beginInfo.clearValueCount = 1;
				beginInfo.pClearValues = &clearValue;

				m_CommandBuffer.BeginRenderPass(&beginInfo);
			}

			ImGui_ImplVulkan_RenderDrawData(drawData, m_CommandBuffer.GetCommandBuffer());

			{ // End RenderPass
				m_CommandBuffer.EndRenderPass();
			}
		}
		m_CommandBuffer.EndCommandBuffer();

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer.GetCommandBuffer();
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_AcquireSephamore.GetSemaphore();
		VkPipelineStageFlags waitMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		submitInfo.pWaitDstStageMask = &waitMask;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_ReleaseSephamore.GetSemaphore();
		VK_ASSERT(
			vkQueueSubmit(
				m_VulkanContext.GetQueue(), 
				1, 
				&submitInfo, 
				m_Fence.GetFence()
			), 
			"Could not submit queue"
		);

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain.GetSwapchain();
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_ReleaseSephamore.GetSemaphore();
		{
			VkResult result = vkQueuePresentKHR(m_VulkanContext.GetQueue(), &presentInfo);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				// Swapchain out of date
				RecreateSwapchain();
			}
			else
			{
				VK_ASSERT(result, "Could not queue present!");
			}
		}
	}

	void GLFWWindow::InitEventCallbacks()
	{
		glfwSetWindowUserPointer(m_WindowHandle, &m_Data);

		glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (!data.eventCallback)
			{
				return;
			}

			WindowCloseEvent event;
			data.eventCallback(event);
		});

		glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow* window, int32_t width, int32_t height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			if (!data.eventCallback)
			{
				return;
			}

			WindowResizeEvent event(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
			data.eventCallback(event);
		});

		glfwSetWindowPosCallback(m_WindowHandle, [](GLFWwindow* window, int32_t xPos, int32_t yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (!data.eventCallback)
			{
				return;
			}

			WindowMovedEvent event(static_cast<uint32_t>(xPos), static_cast<uint32_t>(yPos));
			data.eventCallback(event);
		});

		glfwSetWindowFocusCallback(m_WindowHandle, [](GLFWwindow* window, int32_t focused)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (!data.eventCallback)
			{
				return;
			}

			WindowFocusEvent event(static_cast<bool>(focused));
			data.eventCallback(event);
		});

		glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (!data.eventCallback)
			{
				return;
			}

			switch (action)
			{
				case GLFW_PRESS:
				{
					{
						KeyPressedEvent event(key, false);
						data.eventCallback(event);
					}

					{
						KeyJustPressedEvent event(key);
						data.eventCallback(event);
					}
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.eventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_WindowHandle, [](GLFWwindow* window, uint32_t keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (!data.eventCallback)
			{
				return;
			}

			KeyJustPressedEvent event(keycode);
			data.eventCallback(event);
		});

		glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (!data.eventCallback)
			{
				return;
			}

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.eventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow* window, double xScrollOffset, double yScrollOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (!data.eventCallback)
			{
				return;
			}

			MouseScrolledEvent event(static_cast<float>(xScrollOffset), static_cast<float>(yScrollOffset));
			data.eventCallback(event);
		});

		glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow* window, double xMousePos, double yMousePos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (!data.eventCallback)
			{
				return;
			}

			MouseMovedEvent event(static_cast<float>(xMousePos), static_cast<float>(yMousePos));
			data.eventCallback(event);
		});
	}

	void GLFWWindow::RecreateSwapchain()
	{
		{ // Minimized Check
			VkSurfaceCapabilitiesKHR surfaceCapabilites;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VulkanContext.GetPhysicalDevice(), m_Surface, &surfaceCapabilites);
			if (surfaceCapabilites.currentExtent.width == 0 || surfaceCapabilites.currentExtent.height == 0)
			{
				return;
			}
		}

		VK_ASSERT(vkDeviceWaitIdle(m_VulkanContext.GetDevice()), "Could not wait for device idle!");

		{
			VkSwapchainKHR oldSwapchain = m_Swapchain.GetSwapchain();
			std::vector<VkImageView> oldImageVies = m_Swapchain.GetImageViews();

			m_Swapchain = VulkanSwapchain(
				m_VulkanContext.GetDevice(),
				m_VulkanContext.GetPhysicalDevice(),
				m_VulkanContext.GetQueueFamilyIndex(),
				m_Surface,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				m_Data.vSync,
				oldSwapchain);

			{ // this is just the destroy function of VulkanSwapchain
				for (uint32_t i = 0; i < oldImageVies.size(); i++)
				{
					vkDestroyImageView(m_VulkanContext.GetDevice(), oldImageVies[i], nullptr);
				}
				vkDestroySwapchainKHR(m_VulkanContext.GetDevice(), oldSwapchain, nullptr);
			}
		}

		{
			m_RenderPass.Destroy(m_VulkanContext.GetDevice());

			m_RenderPass = VulkanRenderPass(m_VulkanContext.GetDevice(), m_Swapchain.GetFormat());
		}

		{
			for (uint32_t i = 0; i < m_FrameBuffers.size(); i++)
			{
				m_FrameBuffers[i].Destroy(m_VulkanContext.GetDevice());
			}
			m_FrameBuffers.clear();

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
		}
	}

	void GLFWWindow::ErrorCallback(
		int error, 
		const char* description
	)
	{
		fprintf(stderr, "GLFW Error %d: %s\n", error, description);
	}
}
#endif

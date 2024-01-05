#include "imvkpch.h"
#ifdef IMVK_PLATFORM_GLFW
#include "ImVulkan/Platform/GLFW/GLFWWindow.h"

#include "ImVulkan/Core/Core.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "ImVulkan/Event/KeyboardEvent.h"
#include "ImVulkan/Event/MouseEvent.h"
#include "ImVulkan/Event/WindowEvent.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

namespace ImVulkan
{
	Window* Window::Create(const WindowSpecification& spec)
	{
		return new GLFWWindow(spec);
	}

	uint32_t GLFWWindow::instanceExtensionCount = UINT32_MAX;
	const char** GLFWWindow::instanceExtensions = nullptr;
	uint32_t GLFWWindow::deviceExtensionCount = UINT32_MAX;
	const char** GLFWWindow::deviceExtensions = nullptr;

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
	}

	void GLFWWindow::Destroy(VkInstance instance, VkDevice device)
	{
		VK_ASSERT(vkDeviceWaitIdle(device), "Something went wrong when waiting on device idle!");

		{
			m_ImGuiContext.Destroy(device);
		}
		
		{
			m_AcquireSephamore.Destroy(device);
			m_ReleaseSephamore.Destroy(device);
			m_CommandPool.Destroy(device);
		}

		{
			m_RenderPass.Destroy(device);
			m_Swapchain.Destroy(device);
			vkDestroySurfaceKHR(instance, m_Surface, nullptr);
		}

		{
			for (uint32_t i = 0; i < m_FrameBuffers.size(); i++)
			{
				m_FrameBuffers[i].Destroy(device);
			}
			m_FrameBuffers.clear();
		}

		{
			glfwDestroyWindow(m_WindowHandle);
			glfwTerminate();
		}
	}

	void GLFWWindow::InitVulkan(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndex, VkQueue queue)
	{
		IMVK_ASSERT(!glfwVulkanSupported(), "GLFW: Vulkan is not supported!");

		{
			glfwCreateWindowSurface(instance, m_WindowHandle, nullptr, &m_Surface);

			m_Swapchain = VulkanSwapchain(
				device,
				physicalDevice,
				queueFamilyIndex,
				m_Surface,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				m_Data.vSync
			);
		}

		{
			m_RenderPass = VulkanRenderPass(device, m_Swapchain.GetFormat());
		}

		m_FrameBuffers.resize(m_Swapchain.GetImages().size());
		for (uint32_t i = 0; i < m_Swapchain.GetImages().size(); i++)
		{
			m_FrameBuffers[i] = VulkanFrameBuffer(
				device,
				m_RenderPass.GetRenderPass(),
				m_Swapchain.GetImageViews()[i],
				m_Swapchain.GetWidth(),
				m_Swapchain.GetHeight()
			);
		}

		{
			m_AcquireSephamore = VulkanSemaphore(device);
			m_ReleaseSephamore = VulkanSemaphore(device);
		}

		{
			m_CommandPool = VulkanCommandPool(device, queueFamilyIndex);
		}

		{
			m_CommandBuffer = VulkanCommandBuffer(device, m_CommandPool.GetCommandPool());
		}

		m_ImGuiContext = GLFWImGuiContext(
			m_WindowHandle,
			instance,
			physicalDevice,
			device,
			queueFamilyIndex,
			queue,
			m_Swapchain.GetImages().size(),
			m_RenderPass.GetRenderPass()
		);
	}

	void GLFWWindow::GetInstanceExtensions(
		uint32_t& outInstanceExtensionCount, 
		const char**& outInstanceExtensions
	)
	{
		if (GLFWWindow::instanceExtensionCount == UINT32_MAX)
		{
			GLFWWindow::instanceExtensions = glfwGetRequiredInstanceExtensions(&GLFWWindow::instanceExtensionCount);
		}

		outInstanceExtensionCount = GLFWWindow::instanceExtensionCount;
		outInstanceExtensions = GLFWWindow::instanceExtensions;
	}

	void GLFWWindow::GetDeviceExtensions(
		uint32_t& outDeviceExtensionCount, 
		const char**& outDeviceExtensions
	)
	{
		if (GLFWWindow::deviceExtensionCount == UINT32_MAX)
		{
			GLFWWindow::deviceExtensions = new const char* []
			{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			GLFWWindow::deviceExtensionCount = ARRAY_COUNT(GLFWWindow::deviceExtensions);
		}

		outDeviceExtensionCount = GLFWWindow::deviceExtensionCount;
		outDeviceExtensions = GLFWWindow::deviceExtensions;
	}

	void GLFWWindow::Resize(
		uint32_t width,
		uint32_t height,
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		uint32_t queueFamilyIndex
	)
	{
		if (m_Data.width == width && m_Data.height == height)
		{
			return;
		}

		m_Data.width = width;
		m_Data.height = height;

		glfwSetWindowSize(m_WindowHandle, width, height);

		RecreateSwapchain(physicalDevice, device, queueFamilyIndex);
	}

	void GLFWWindow::PollEvents()
	{
		glfwPollEvents();
	}

	const bool GLFWWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_WindowHandle);
	}

	const bool GLFWWindow::AcquireNextImage(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		uint32_t queueFamilyIndex
	)
	{
		if (m_Data.minimized)
		{
			return false;
		}

		{
			// if acquire next image gets called when the window is minimized it will lead to a validation error
			// the only way to not make that happen is to return early when the window is minimized

			VkResult result = vkAcquireNextImageKHR(
				device,
				m_Swapchain.GetSwapchain(),
				UINT64_MAX, m_AcquireSephamore.GetSemaphore(),
				nullptr,
				&m_ImageIndex
			);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				RecreateSwapchain(physicalDevice, device, queueFamilyIndex);
				return false;
			}
			else
			{
				VK_ASSERT(result, "Could not acquire next image")
			}
		}

		return true;
	}

	void GLFWWindow::BeginImGuiFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	}

	ImDrawData* GLFWWindow::EndImGuiFrame()
	{
		ImGui::Render();
		return ImGui::GetDrawData();
	}

	void GLFWWindow::SwapBuffers(
		ImDrawData* imGuiDrawData,
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		uint32_t queueFamilyIndex,
		VkQueue queue,
		VkFence fence
	)
	{
		m_CommandPool.Reset(device);

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
				beginInfo.framebuffer = m_FrameBuffers[m_ImageIndex].GetFrameBuffer();
				beginInfo.renderArea = { {0, 0}, {m_Swapchain.GetWidth(), m_Swapchain.GetHeight()} };
				beginInfo.clearValueCount = 1;
				beginInfo.pClearValues = &clearValue;

				m_CommandBuffer.BeginRenderPass(&beginInfo);
			}

			{
				ImGui_ImplVulkan_RenderDrawData(imGuiDrawData, m_CommandBuffer.GetCommandBuffer());
			}

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
				queue, 
				1, 
				&submitInfo, 
				fence
			), 
			"Could not submit queue"
		);

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain.GetSwapchain();
		presentInfo.pImageIndices = &m_ImageIndex;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_ReleaseSephamore.GetSemaphore();
		{
			VkResult result = vkQueuePresentKHR(queue, &presentInfo);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				// Swapchain out of date
				RecreateSwapchain(physicalDevice, device, queueFamilyIndex);
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
			
			if (width == 0 && height == 0)
			{
				data.minimized = true;
			}
			else
			{
				data.minimized = false;
			}

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

	void GLFWWindow::RecreateSwapchain(
		VkPhysicalDevice physicalDevice, 
		VkDevice device, 
		uint32_t queueFamilyIndex
	)
	{
		{ // Minimized Check
			VkSurfaceCapabilitiesKHR surfaceCapabilites;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfaceCapabilites);
			if (surfaceCapabilites.currentExtent.width == 0 || surfaceCapabilites.currentExtent.height == 0)
			{
				return;
			}
		}

		VK_ASSERT(vkDeviceWaitIdle(device), "Could not wait for device idle!");

		{
			VkSwapchainKHR oldSwapchain = m_Swapchain.GetSwapchain();
			std::vector<VkImageView> oldImageVies = m_Swapchain.GetImageViews();

			m_Swapchain = VulkanSwapchain(
				device,
				physicalDevice,
				queueFamilyIndex,
				m_Surface,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				m_Data.vSync,
				oldSwapchain);

			{ // this is just the destroy function of VulkanSwapchain
				for (uint32_t i = 0; i < oldImageVies.size(); i++)
				{
					vkDestroyImageView(device, oldImageVies[i], nullptr);
				}
				vkDestroySwapchainKHR(device, oldSwapchain, nullptr);
			}
		}

		{
			m_RenderPass.Destroy(device);

			m_RenderPass = VulkanRenderPass(device, m_Swapchain.GetFormat());
		}

		{
			for (uint32_t i = 0; i < m_FrameBuffers.size(); i++)
			{
				m_FrameBuffers[i].Destroy(device);
			}
			m_FrameBuffers.clear();

			m_FrameBuffers.resize(m_Swapchain.GetImages().size());
			for (uint32_t i = 0; i < m_Swapchain.GetImages().size(); i++)
			{
				m_FrameBuffers[i] = VulkanFrameBuffer(
					device,
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

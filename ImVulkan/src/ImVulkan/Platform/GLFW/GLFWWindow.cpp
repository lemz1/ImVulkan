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

	uint32_t GLFWWindow::instanceExtensionCount = 0;
	const char** GLFWWindow::instanceExtensions = nullptr;
	uint32_t GLFWWindow::deviceExtensionCount = 0;
	const char** GLFWWindow::deviceExtensions = nullptr;

	GLFWWindow::GLFWWindow(const WindowSpecification& spec)
	{
		m_Data.title = spec.title;
		m_Data.width = spec.width;
		m_Data.height = spec.height;
		m_Data.vSync = spec.vSync;
		m_Data.minimized = false;
		m_Data.eventCallback = nullptr;
		m_Data.recreateSwapchain = nullptr;

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
			vkDestroyRenderPass(device, m_RenderPass, nullptr);
			m_Swapchain.Destroy(device);
			vkDestroySurfaceKHR(instance, m_Surface, nullptr);
		}

		{
			for (uint32_t i = 0; i < m_FrameBuffers.size(); i++)
			{
				vkDestroyFramebuffer(device, m_FrameBuffers[i], nullptr);
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
			m_RenderPass = VulkanRenderPass::Create(device, m_Swapchain.format);
		}

		{
			m_FrameBuffers.resize(m_Swapchain.images.size());
			for (uint32_t i = 0; i < m_Swapchain.images.size(); i++)
			{
				m_FrameBuffers[i] = VulkanFrameBuffer::Create(
					device,
					m_RenderPass,
					m_Swapchain.imageViews[i],
					m_Swapchain.width,
					m_Swapchain.height
				);
			}
		}

		m_ImGuiContext = GLFWImGuiContext(
			m_WindowHandle,
			instance,
			physicalDevice,
			device,
			queueFamilyIndex,
			queue,
			m_Swapchain.images.size(),
			m_RenderPass
		);
	}

	void GLFWWindow::GetInstanceExtensions(
		uint32_t& outInstanceExtensionCount, 
		const char**& outInstanceExtensions
	)
	{
		if (GLFWWindow::instanceExtensionCount == 0)
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
		if (GLFWWindow::deviceExtensionCount == 0)
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
		uint32_t queueFamilyIndex,
		VkSemaphore signalSemaphore
	)
	{
		// if you resize your window and make the height 0 it wont count as minimized 
		// that's why here is also the check for width and height
		if (m_Data.minimized || m_Data.width == 0 || m_Data.height == 0)
		{
			return false;
		}

		VK_ASSERT(
			vkAcquireNextImageKHR(
				device,
				m_Swapchain.swapchain,
				UINT64_MAX, 
				signalSemaphore,
				nullptr,
				&m_ImageIndex
			),
			"Could not acquire next image!"
		);

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
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		uint32_t queueFamilyIndex,
		VkQueue queue,
		VkFence fence,
		VkSemaphore waitSemaphore
	)
	{
		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain.swapchain;
		presentInfo.pImageIndices = &m_ImageIndex;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &waitSemaphore;
		VK_ASSERT(
			vkQueuePresentKHR(
				queue,
				&presentInfo
			),
			"Could not present image!"
		);
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
				data.recreateSwapchain();
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
		{ // if width or height is 0 return early
			VkSurfaceCapabilitiesKHR surfaceCapabilites;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfaceCapabilites);
			if (surfaceCapabilites.currentExtent.width == 0 || surfaceCapabilites.currentExtent.height == 0)
			{
				return;
			}
		}
		
		VK_ASSERT(vkDeviceWaitIdle(device), "Could not wait for device idle!");

		{
			VkSwapchainKHR oldSwapchain = m_Swapchain.swapchain;
			std::vector<VkImageView> oldImageVies = m_Swapchain.imageViews;

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
			vkDestroyRenderPass(device, m_RenderPass, nullptr);

			m_RenderPass = VulkanRenderPass::Create(device, m_Swapchain.format);
		}

		{
			for (uint32_t i = 0; i < m_FrameBuffers.size(); i++)
			{
				vkDestroyFramebuffer(device, m_FrameBuffers[i], nullptr);
			}
			m_FrameBuffers.clear();

			m_FrameBuffers.resize(m_Swapchain.images.size());
			for (uint32_t i = 0; i < m_Swapchain.images.size(); i++)
			{
				m_FrameBuffers[i] = VulkanFrameBuffer::Create(
					device,
					m_RenderPass,
					m_Swapchain.imageViews[i],
					m_Swapchain.width,
					m_Swapchain.height
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

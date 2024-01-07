#include "imvkpch.h"
#include "ImVulkan/Core/Application.h"

#include "ImVulkan/Util/Time.h"
#include "ImVulkan/Vulkan/VulkanSemaphore.h"
#include "ImVulkan/Vulkan/VulkanCommandPool.h"
#include "ImVulkan/Vulkan/Buffer/VulkanCommandBuffer.h"

#include "backends/imgui_impl_vulkan.h"

namespace ImVulkan
{
	Application* Application::s_Instance = nullptr;

	Application* Application::Create(const ApplicationSpecification& spec)
	{
		if (Application::s_Instance)
		{
			return Application::s_Instance;
		}

		return new Application(spec);
	}

	Application::Application(const ApplicationSpecification& spec)
	{
		Time::Init();

		Application::s_Instance = this;

		#ifndef IMVK_HEADLESS
		m_Window = Window::Create(spec.windowSpec);
		{
			uint32_t instanceExtensionCount = 0;
			const char** instanceExtensions;
			m_Window->GetInstanceExtensions(instanceExtensionCount, instanceExtensions);

			const char* additionalInstanceExtensions[] = {
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
				VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME
			};

			uint32_t mergedInstanceExtensionCount = instanceExtensionCount + ARRAY_COUNT(additionalInstanceExtensions);
			const char** mergedInstanceExtensions = new const char* [mergedInstanceExtensionCount];

			memcpy(
				mergedInstanceExtensions, 
				instanceExtensions, 
				instanceExtensionCount * sizeof(const char*)
			);

			memcpy(
				mergedInstanceExtensions + instanceExtensionCount, 
				additionalInstanceExtensions, 
				ARRAY_COUNT(additionalInstanceExtensions) * sizeof(const char*)
			);

			uint32_t deviceExtensionCount = 0;
			const char** deviceExtensions;
			m_Window->GetDeviceExtensions(deviceExtensionCount, deviceExtensions);

			m_VulkanContext = VulkanContext(
				mergedInstanceExtensionCount,
				mergedInstanceExtensions,
				deviceExtensionCount, 
				deviceExtensions
			);

			delete[] mergedInstanceExtensions;
		}

		m_Window->InitVulkan(
			m_VulkanContext.instance,
			m_VulkanContext.physicalDevice,
			m_VulkanContext.device,
			m_VulkanContext.queueFamilyIndex,
			m_VulkanContext.queue
		);

		m_Window->SetEventCallback(
			std::bind(
				&Application::OnEvent, 
				this, 
				std::placeholders::_1
			)
		);
		m_Window->SetRecreateSwapchain(
			std::bind(
				&Application::RecreateSwapchain, 
				this
			)
		);

		m_ImGuiCommandPool = VulkanCommandPool::Create(m_VulkanContext.device, m_VulkanContext.queueFamilyIndex);

		m_ImGuiCommandBuffer = VulkanCommandBuffer::Create(m_VulkanContext.device, m_ImGuiCommandPool);
		#else
		m_VulkanContext = new VulkanContext(
			0,
			nullptr,
			0,
			nullptr
		);
		#endif

		m_VulkanDebugMessenger = VulkanDebugMessenger::Create(m_VulkanContext.instance);
	}

	Application::~Application()
	{
		VK_ASSERT(vkDeviceWaitIdle(m_VulkanContext.device), "Could not wait for device idle!");

		#ifndef IMVK_HEADLESS
		vkDestroyCommandPool(m_VulkanContext.device, m_ImGuiCommandPool, nullptr);

		for (VkSemaphore semaphore : m_Semaphores)
		{
			vkDestroySemaphore(m_VulkanContext.device, semaphore, nullptr);
		}
		m_Semaphores.clear();

		m_Window->Destroy(m_VulkanContext.instance, m_VulkanContext.device);
		delete m_Window;
		#endif

		m_LayerStack.Destroy();

		VulkanDebugMessenger::Destroy(m_VulkanDebugMessenger, m_VulkanContext.instance);

		m_VulkanContext.Destroy();
		
		Application::s_Instance = nullptr;
	}

	void Application::Run()
	{
		m_IsRunning = true;
		double time = 0;

		while (true)
		{
			#ifndef IMVK_HEADLESS
			m_Window->PollEvents();

			if (m_Window->ShouldClose())
			{
				break;
			}
			#endif

			const double newTime = Time::GetTime();
			const double deltaTime = newTime - time;
			time = newTime;

			m_LayerStack.OnUpdate(deltaTime);

			#ifndef IMVK_HEADLESS

			VkSemaphore imageSemaphore = VulkanSemaphore::Create(m_VulkanContext.device);

			if (!m_Window->AcquireNextImage(
					m_VulkanContext.physicalDevice, 
					m_VulkanContext.device, 
					m_VulkanContext.queueFamilyIndex,
					imageSemaphore
				)
			)
			{
				vkDestroySemaphore(m_VulkanContext.device, imageSemaphore, nullptr);
				continue;
			}

			VK_ASSERT(
				vkWaitForFences(
					m_VulkanContext.device,
					1,
					&m_VulkanContext.fence,
					VK_TRUE,
					UINT64_MAX
				),
				"Could not wait for fence!"
			);

			for (VkSemaphore semaphore : m_Semaphores)
			{
				vkDestroySemaphore(m_VulkanContext.device, semaphore, nullptr);
			}
			m_Semaphores.clear();
			m_Semaphores.push_back(imageSemaphore);

			VK_ASSERT(
				vkResetFences(
					m_VulkanContext.device,
					1,
					&m_VulkanContext.fence
				),
				"Could not reset fence!"
			);

			m_LayerStack.OnDraw();

			m_Window->BeginImGuiFrame();

			m_LayerStack.OnDrawImGui();

			ImDrawData* imguiDrawData = m_Window->EndImGuiFrame();

			//ImGuiCommandBuffer(imguiDrawData);

			SubmitCommandBuffers(m_VulkanContext.fence);

			m_Window->SwapBuffers(
				m_VulkanContext.physicalDevice,
				m_VulkanContext.device,
				m_VulkanContext.queueFamilyIndex,
				m_VulkanContext.queue,
				m_VulkanContext.fence,
				m_Semaphores.at(m_Semaphores.size() - 1)
			);
			#endif
		}

		m_IsRunning = false;
	}

	void Application::OnEvent(Event& event)
	{
		#ifndef IMVK_HEADLESS
		m_LayerStack.OnEvent(event);
		#endif
	}

	void Application::RecreateSwapchain()
	{
		#ifndef IMVK_HEADLESS
		m_Window->RecreateSwapchain(
			m_VulkanContext.physicalDevice,
			m_VulkanContext.device,
			m_VulkanContext.queueFamilyIndex
		);
		#endif
	}

	void Application::AddCommandBuffer(VkCommandBuffer commandBuffer)
	{
		s_Instance->m_CommandBuffers.push_back(commandBuffer);

		if (s_Instance->m_CommandBuffers.size() == NUM_CMDBUF_FOR_SUBMIT)
		{
			s_Instance->SubmitCommandBuffers(nullptr);
		}
	}

	void Application::ImGuiCommandBuffer(ImDrawData* drawData)
	{
		VK_ASSERT(
			vkResetCommandPool(
				m_VulkanContext.device, 
				m_ImGuiCommandPool, 
				0
			),
			"Could not reset command pool!"
		);

		{
			VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			VK_ASSERT(
				vkBeginCommandBuffer(
					m_ImGuiCommandBuffer,
					&beginInfo
				),
				"Could not begin command buffer!"
			);
		}

		{
			{
				VkViewport viewport = { 0.f, 0.f, (float)m_Window->GetWidth(), (float)m_Window->GetHeight() };
				VkRect2D scissor = { {0, 0}, {m_Window->GetWidth(), m_Window->GetHeight()} };

				vkCmdSetViewport(
					m_ImGuiCommandBuffer,
					0,
					1,
					&viewport
				);

				vkCmdSetScissor(
					m_ImGuiCommandBuffer,
					0,
					1,
					&scissor
				);
			}

			{
				VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
				beginInfo.renderPass = m_Window->GetRenderPass();
				beginInfo.framebuffer = m_Window->GetCurrentFrameBuffer();
				beginInfo.renderArea = { {0, 0}, {m_Window->GetWidth(), m_Window->GetHeight()} };

				vkCmdBeginRenderPass(
					m_ImGuiCommandBuffer,
					&beginInfo,
					VK_SUBPASS_CONTENTS_INLINE
				);
			}

			{
				ImGui_ImplVulkan_RenderDrawData(drawData, m_ImGuiCommandBuffer);
			}

			{
				vkCmdEndRenderPass(m_ImGuiCommandBuffer);
			}
		}
		
		{
			vkEndCommandBuffer(m_ImGuiCommandBuffer);
		}

		AddCommandBuffer(m_ImGuiCommandBuffer);
	}

	void Application::SubmitCommandBuffers(VkFence fence)
	{
		/*
		if (m_CommandBuffers.size() == 0)
		{
			return;
		}
		*/

		VkSemaphore signalSemaphore = VulkanSemaphore::Create(m_VulkanContext.device);
		m_Semaphores.push_back(signalSemaphore);

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = s_Instance->m_CommandBuffers.size();
		submitInfo.pCommandBuffers = s_Instance->m_CommandBuffers.data();
		VkPipelineStageFlags waitMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		submitInfo.pWaitDstStageMask = &waitMask;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_Semaphores.at(m_Semaphores.size() - 2);
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;
		VK_ASSERT(
			vkQueueSubmit(
				s_Instance->m_VulkanContext.queue,
				1,
				&submitInfo,
				fence
			),
			"Could not submit queue"
		);

		m_CommandBuffers.clear();
	}
}

#include "imvkpch.h"
#include "ImVulkan/Core/Application.h"

#include "ImVulkan/Util/Time.h"

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
		#ifndef IMVK_HEADLESS
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
			if (!m_Window->AcquireNextImage(
					m_VulkanContext.physicalDevice, 
					m_VulkanContext.device, 
					m_VulkanContext.queueFamilyIndex
				)
			)
			{
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

			ImDrawData* imGuiDrawData = m_Window->EndImGuiFrame();

			m_Window->SwapBuffers(
				imGuiDrawData,
				m_VulkanContext.physicalDevice,
				m_VulkanContext.device,
				m_VulkanContext.queueFamilyIndex,
				m_VulkanContext.queue,
				m_VulkanContext.fence
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
}

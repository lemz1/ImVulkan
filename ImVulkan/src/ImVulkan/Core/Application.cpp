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

		Application::s_Instance = this;

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
			m_VulkanContext.GetInstance(),
			m_VulkanContext.GetPhysicalDevice(),
			m_VulkanContext.GetDevice(),
			m_VulkanContext.GetQueueFamilyIndex(),
			m_VulkanContext.GetQueue()
		);

		m_Window->SetEventCallback(BindFunction(&Application::OnEvent, this));
	}

	Application::~Application()
	{
		m_Window->Destroy(m_VulkanContext.GetInstance(), m_VulkanContext.GetDevice());
		delete m_Window;

		m_VulkanContext.Destroy();

		Application::s_Instance = nullptr;
	}

	void Application::Run()
	{
		double time = 0;

		while (true)
		{
			const double newTime = Time::GetTime();
			const double deltaTime = newTime - time;
			time = newTime;

			m_Window->PollEvents();

			if (m_Window->ShouldClose())
			{
				break;
			}

			m_LayerStack.OnUpdate(deltaTime);

			m_LayerStack.OnDraw();

			m_Window->BeginImGuiFrame();

			m_LayerStack.OnDrawImGui();

			ImDrawData* imGuiDrawData = m_Window->EndImGuiFrame();

			m_Window->SwapBuffers(
				imGuiDrawData,
				m_VulkanContext.GetPhysicalDevice(),
				m_VulkanContext.GetDevice(),
				m_VulkanContext.GetQueueFamilyIndex(),
				m_VulkanContext.GetQueue()
			);
		}
	}

	void Application::OnEvent(Event& event)
	{
		m_LayerStack.OnEvent(event);
	}
}

#pragma once

#include "ImVulkan/Core/Window.h"
#include "ImVulkan/Core/LayerStack.h"
#include "ImVulkan/Platform/Vulkan/VulkanContext.h"

namespace ImVulkan
{
	struct ApplicationSpecification
	{
		WindowSpecification windowSpec;
	};

	class Application
	{
	public:
		static Application* Create(const ApplicationSpecification& spec);
		~Application();

		void Run();

		void OnEvent(Event& event);

		void RecreateSwapchain();

		static void PushLayer(Layer* layer) { s_Instance->m_LayerStack.PushLayer(layer); }
		static void RemoveLayer(Layer* layer) { s_Instance->m_LayerStack.RemoveLayer(layer); }
		static const std::vector<Layer*>& GetLayers() { return s_Instance->m_LayerStack.GetLayers(); }

		static const Window* GetWindow() { return s_Instance->m_Window; }

		static const Application* GetInstance() { return s_Instance; }

		static const VulkanContext& GetVulkanContext() { return s_Instance->m_VulkanContext; }
	private:
		Application(const ApplicationSpecification& spec);
	private:
		VulkanContext m_VulkanContext;
		VkDebugUtilsMessengerEXT m_VulkanDebugMessenger;

		Window* m_Window = nullptr;
		LayerStack m_LayerStack;

		bool m_IsRunning;
	private:
		static Application* s_Instance;
	};
}

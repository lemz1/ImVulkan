#pragma once

#include "ImVulkan/Core/Window.h"
#include "ImVulkan/Core/LayerStack.h"
#include "ImVulkan/Vulkan/VulkanContext.h"

#define NUM_CMDBUF_FOR_SUBMIT 3

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

		static void AddCommandBuffer(VkCommandBuffer commandBuffer);

		static const Window* GetWindow() { return s_Instance->m_Window; }

		static const Application* GetInstance() { return s_Instance; }

		static const VulkanContext& GetVulkanContext() { return s_Instance->m_VulkanContext; }
	private:
		Application(const ApplicationSpecification& spec);

		void ImGuiCommandBuffer(ImDrawData* drawData);

		void SubmitCommandBuffers(VkFence fence);
	private:
		VulkanContext m_VulkanContext;
		VkDebugUtilsMessengerEXT m_VulkanDebugMessenger;

		Window* m_Window = nullptr;
		LayerStack m_LayerStack;

		VkCommandPool m_ImGuiCommandPool;
		VkCommandBuffer m_ImGuiCommandBuffer;

		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<VkSemaphore> m_Semaphores;

		bool m_IsRunning;
	private:
		static Application* s_Instance;
	};
}

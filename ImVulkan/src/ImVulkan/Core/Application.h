#pragma once

#include "ImVulkan/Core/Window.h"
#include "ImVulkan/Core/LayerStack.h"

namespace ImVulkan
{
	struct ApplicationSpecification
	{
		WindowSpecification windowSpec;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& spec);
		~Application();

		void Run();

		void PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }
		void RemoveLayer(Layer* layer) { m_LayerStack.RemoveLayer(layer); }
		const std::vector<Layer*>& GetLayers() const { return m_LayerStack.GetLayers(); }
	public:
		static Application* s_Instance;
	private:
		Window* m_Window;
		LayerStack m_LayerStack;
	};
}

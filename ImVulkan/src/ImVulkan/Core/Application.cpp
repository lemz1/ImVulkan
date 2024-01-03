#include "imvkpch.h"
#include "ImVulkan/Core/Application.h"

#include "ImVulkan/Util/Time.h"

namespace ImVulkan
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& spec)
	{
		m_Window = Window::Create(spec.windowSpec);
		m_Window->SetEventCallback(BindFunction(&Application::OnEvent, this));
	}

	Application::~Application()
	{
		delete m_Window;
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

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& event)
	{
		m_LayerStack.OnEvent(event);
	}
}

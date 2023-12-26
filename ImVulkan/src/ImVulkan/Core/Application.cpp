#include "imvkpch.h"
#include "ImVulkan/Core/Application.h"

namespace ImVulkan
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& spec)
	{
		m_Window = Window::Create(spec.windowSpec);
	}

	Application::~Application()
	{
		delete m_Window;
	}

	void Application::Run()
	{
		while (true)
		{
			m_Window->PollEvents();

			if (m_Window->ShouldClose())
			{
				break;
			}

			m_Window->OnUpdate();
		}
	}
}

#pragma once

#include "ImVulkan/Core/Window.h"

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
	public:
		static Application* s_Instance;
	private:
		Window* m_Window;
	};
}

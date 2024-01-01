#include "ImVulkan.h"

namespace ImVulkan
{
	class ExampleLayer : public Layer
	{
	public:
		void OnCreate() override
		{
			IMVK_INFO("Example Layer Create!");
			IMVK_WARN("Example Layer Create!");
			IMVK_ERROR("Example Layer Create!");
		}
	};

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpecification spec;
		Application* app = new Application(spec);
		app->PushLayer(new ExampleLayer());
		return app;
	}
}

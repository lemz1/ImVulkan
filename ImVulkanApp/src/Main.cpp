#include "ImVulkan.h"

namespace ImVulkan
{

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpecification spec;
		Application* app = new Application(spec);
		return app;
	}
}
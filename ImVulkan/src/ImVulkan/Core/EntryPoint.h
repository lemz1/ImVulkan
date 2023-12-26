#pragma once

#include "ImVulkan/Core/Application.h"

namespace ImVulkan
{
	extern Application* CreateApplication(int argc, char** argv);

	int Main(int argc, char** argv)
	{
		Application* app = CreateApplication(argc, argv);
		app->Run();
		delete app;
		return 0;
	}
}

int main(int argc, char** argv)
{
	return ImVulkan::Main(argc, argv);
}

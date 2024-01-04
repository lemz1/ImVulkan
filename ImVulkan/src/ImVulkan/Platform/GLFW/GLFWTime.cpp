#include "imvkpch.h"
#ifdef IMVK_PLATFORM_GLFW
#include "ImVulkan/Util/Time.h"

#include <GLFW/glfw3.h>

namespace ImVulkan 
{
	void Time::Init() {}

	double Time::GetTime()
	{
		return glfwGetTime();
	}

	double Time::GetTimeMS()
	{
		return glfwGetTime() * 1000;
	}
}
#endif

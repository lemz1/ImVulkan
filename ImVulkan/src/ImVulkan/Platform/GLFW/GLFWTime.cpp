#include "imvkpch.h"
#ifdef IMVK_PLATFORM_GLFW
#include "ImVulkan/Util/Time.h"

#include <GLFW/glfw3.h>

namespace ImVulkan::Time
{
	double GetTime()
	{
		return glfwGetTime();
	}

	uint64_t GetTimeMS()
	{
		return glfwGetTime() * 1000;
	}
}
#endif

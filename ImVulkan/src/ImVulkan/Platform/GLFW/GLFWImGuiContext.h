#pragma once

#include "ImVulkan/Platform/GLFW/GLFWWindow.h"

namespace ImVulkan
{
	class GLFWImGuiContext
	{
	public:
		GLFWImGuiContext() = default;
		GLFWImGuiContext(GLFWWindow* window);
	private:
	};
}

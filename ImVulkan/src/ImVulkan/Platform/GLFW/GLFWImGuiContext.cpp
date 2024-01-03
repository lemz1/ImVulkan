#include "imvkpch.h"
#include "GLFWImGuiContext.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"

namespace ImVulkan
{
	GLFWImGuiContext::GLFWImGuiContext(GLFWWindow* window)
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= 
		ImGui::StyleColorsDark();
	}
}

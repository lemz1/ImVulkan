#pragma once

#include "ImVulkan/Event/Event.h"

#include <stdint.h>

namespace ImVulkan
{
	using EventCallback = std::function<void(Event&)>;

	struct WindowSpecification
	{
		const char* title = "ImVulkan Application";
		uint32_t width = 1280, height = 720;
		bool vSync = true;
	};

	class Window
	{
	public:
		static Window* Create(const WindowSpecification& spec);

		virtual ~Window() = default;

		// pointer to HWND, GLFWwindow, etc.
		virtual void* GetNativeWindow() = 0;

		virtual const char* GetTitle() = 0;
		virtual void SetTitle(const char* title) = 0;

		virtual const uint32_t GetWidth() = 0;
		virtual const uint32_t GetHeight() = 0;

		virtual void Resize(
			uint32_t width, 
			uint32_t height
		) = 0;

		virtual const bool GetVSync() const = 0;
		virtual void SetVSync(bool vSync) = 0;

		virtual void PollEvents() = 0;

		virtual const bool ShouldClose() const = 0;

		// swap buffers, etc.
		virtual void OnUpdate() = 0;

		virtual void SetEventCallback(const EventCallback& callback) = 0;
	};
}

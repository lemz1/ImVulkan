#pragma once

#include "ImVulkan/Event/Event.h"

#include <stdint.h>
#include <sstream>

namespace ImVulkan
{

	class WindowCloseEvent : public Event
	{
	public:
		EventClassType(WindowClose)
		EventClassFlag(EventFlagKeyboard)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(const uint32_t width, const uint32_t height) 
			: m_Width(width), m_Height(height)
		{}

		const uint32_t GetWidth() const { return m_Width; }
		const uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent (width, height): " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EventClassType(WindowResize)
		EventClassFlag(EventFlagWindow)
	private:
		uint32_t m_Width, m_Height;
	};

	class WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(const uint32_t windowPosX, const uint32_t windowPosY)
			: m_XWindowPos(windowPosX), m_YWindowPos(windowPosY)
		{}

		const uint32_t GetWindowPosX() const { return m_XWindowPos; }
		const uint32_t GetWindowPosY() const { return m_YWindowPos; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent (x, y): " << m_XWindowPos << ", " << m_YWindowPos;
			return ss.str();
		}

		EventClassType(WindowMoved)
		EventClassFlag(EventFlagWindow)
	private:
		uint32_t m_XWindowPos, m_YWindowPos;
	};

	class WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent(bool focus)
			: m_Focus(focus)
		{}

		const bool IsFocused() const { return m_Focus; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowFocusEvent: " << m_Focus;
			return ss.str();
		}

		EventClassType(WindowFocus)
		EventClassFlag(EventFlagWindow)
	private:
		bool m_Focus;
	};
}

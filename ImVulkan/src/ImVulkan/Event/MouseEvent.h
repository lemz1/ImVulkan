#pragma once

#include "ImVulkan/Event/Event.h"
#include "ImVulkan/Input/MouseCodes.h"

#include <sstream>

namespace ImVulkan
{
	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y) 
			: m_XMousePos(x), m_YMousePos(y)
		{}

		const float GetX() const { return m_XMousePos; }
		const float GetY() const { return m_YMousePos; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent (x, y): " << m_XMousePos << ", " << m_YMousePos;
			return ss.str();
		}

		EventClassType(MouseMoved)
		EventClassFlag(EventFlagMouse)
	private:
		float m_XMousePos, m_YMousePos;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float xScrollOffset, const float yScrollOffset)
			: m_XScrollOffset(xScrollOffset), m_YScrollOffset(yScrollOffset)
		{}

		float GetXScrollOffset() const { return m_XScrollOffset; }
		float GetYScrollOffset() const { return m_YScrollOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent (x, y): " << m_XScrollOffset << ", " << m_YScrollOffset;
			return ss.str();
		}

		EventClassType(MouseScrolled)
		EventClassFlag(EventFlagMouse)
	private:
		float m_XScrollOffset, m_YScrollOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		MouseCode GetMouseButton() const { return m_Button; }

		EventClassFlag(EventFlagMouse)
	protected:
		MouseButtonEvent(const MouseCode button)
			: m_Button(button)
		{}

		MouseCode m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseCode button)
			: MouseButtonEvent(button)
		{}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EventClassType(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseCode button)
			: MouseButtonEvent(button)
		{}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EventClassType(MouseButtonReleased)
	};
}

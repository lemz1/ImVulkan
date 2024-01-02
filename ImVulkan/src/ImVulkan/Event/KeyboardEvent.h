#pragma once

#include "ImVulkan/Event/Event.h"
#include "ImVulkan/Input/KeyCodes.h"

#include <sstream>

namespace ImVulkan
{
	class KeyboardEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }

		EventClassFlag(EventFlagKeyboard)
	protected:
		KeyboardEvent(const KeyCode keycode)
			: m_KeyCode(keycode)
		{}

		KeyCode m_KeyCode;
	};

	class KeyJustPressedEvent : public KeyboardEvent
	{
	public:
		KeyJustPressedEvent(const KeyCode keycode) 
			: KeyboardEvent(keycode)
		{}

		std::string ToString() const override 
		{ 
			std::stringstream ss;
			ss << "KeyJustPressedEvent: " << m_KeyCode;
			return ss.str(); 
		}

		EventClassType(KeyJustPressed)
	};

	class KeyPressedEvent : public KeyboardEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
			: KeyboardEvent(keycode), m_IsRepeat(isRepeat)
		{}

		bool IsRepeat() { return m_IsRepeat; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (Repeat: " << m_IsRepeat << ")";
			return ss.str();
		}

		EventClassType(KeyPressed)
	private:
		bool m_IsRepeat;
	};

	class KeyReleasedEvent : public KeyboardEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode)
			: KeyboardEvent(keycode)
		{}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EventClassType(KeyReleased)
	};
}

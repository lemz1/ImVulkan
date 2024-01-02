#pragma once

#include "ImVulkan/Core/Core.h"

#include <stdint.h>
#include <functional>
#include <string>
#include <iostream>

namespace ImVulkan
{
	enum class EventType : uint32_t
	{
		None = 0,
		WindowClose, WindowResize, WindowMoved, WindowFocus,
		KeyJustPressed, KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
	};

	enum EventFlag : uint32_t
	{
		None = 0,
		EventFlagWindow = Flag(1),
		EventFlagKeyboard = Flag(2),
		EventFlagMouse = Flag(3),
	};

	#define EventClassType(type) static EventType GetStaticEventType() { return EventType::type; }\
								 EventType GetEventType() const override { return GetStaticEventType(); }\
								 const char* GetName() const override { return #type; }

	#define EventClassFlag(flag) uint32_t GetFlag() const override { return flag; }

	class Event
	{
	public:
		virtual ~Event() = default;

		virtual EventType GetEventType() const = 0;
		virtual uint32_t GetFlag() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsFlag(const EventFlag flag) { return GetFlag() & flag; }
	public:
		bool handled = false;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event) 
			: m_Event(event)
		{}

		const Event& GetEvent() const { return m_Event; }

		template<typename T, typename F>
		bool Dispatch(const F& function)
		{
			static_assert(std::is_base_of<Event, T>::value, "T must be a subclass of Event!");

			if (m_Event.GetEventType() == T::GetStaticEventType())
			{
				function(static_cast<T&>(m_Event));
				m_Event.handled = true;
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& event) 
	{
		return os << event.ToString();
	}
}

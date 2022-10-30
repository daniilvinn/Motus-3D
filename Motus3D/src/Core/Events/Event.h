#pragma once

#include <functional>
#include <Core/Macros.h>
#include <variant>
#include <Core/Logger.h>



enum MOTUS_API EventType
{
	None,
	KeyTyped,
	KeyPressed,
	KeyReleased,
	MouseMoved,
	MouseScrolled,
	MouseButtonPressed,
	MouseButtonReleased,
	WindowClosed,
	WindowResized

};

struct MOTUS_API NoneEvent
{
	const EventType type = None;
	static EventType GetStaticType() { return None; }
};

struct MOTUS_API KeyTypedEvent
{
	const EventType type = KeyTyped;
	int keycode;
	static EventType GetStaticType() { return KeyTyped; }
};

struct MOTUS_API KeyPressedEvent
{
	const EventType type = KeyPressed;
	int keycode;
	int repeatcount;
	static EventType GetStaticType() { return KeyPressed; }
};

struct MOTUS_API KeyReleasedEvent
{
	const EventType type = KeyReleased;
	int keycode;
	static EventType GetStaticType() { return KeyReleased; }
};

struct MOTUS_API MouseMovedEvent
{
	const EventType type = MouseMoved;
	int16_t xpos;
	int16_t ypos;
	static EventType GetStaticType() { return MouseMoved; }
};

struct MOTUS_API MouseScrolledEvent
{
	const EventType type = MouseScrolled;
	double xoffset;
	double yoffset;
	static EventType GetStaticType() { return MouseScrolled; }
};

struct MOTUS_API MouseButtonPressedEvent
{
	const EventType type = MouseButtonPressed;
	int button;
	static EventType GetStaticType() { return MouseButtonPressed; }
};

struct MOTUS_API MouseButtonReleasedEvent
{
	const EventType type = MouseButtonReleased;
	int button;
	static EventType GetStaticType() { return MouseButtonReleased; }
};

struct MOTUS_API WindowClosedEvent
{
	const EventType type = WindowClosed;
	static EventType GetStaticType() { return WindowClosed; }
};

struct MOTUS_API WindowResizedEvent
{
	const EventType type = WindowResized;
	int width;
	int height;
	static EventType GetStaticType() { return WindowResized; }
};

// Type alias
using EventVariants = std::variant<NoneEvent, KeyTypedEvent, KeyPressedEvent, KeyReleasedEvent, MouseMovedEvent, MouseScrolledEvent,
									MouseButtonPressedEvent, MouseButtonReleasedEvent, WindowClosedEvent, WindowResizedEvent>;

class Event
{
public:
	Event()
	{
		m_Variants.emplace<NoneEvent>(NoneEvent());
		m_Type = None;
	}

	Event(EventVariants variant, EventType type)
	{
		switch (type)
		{
		case None:
			m_Variants.emplace<NoneEvent>(std::get<NoneEvent>(variant));
			break;
		case KeyTyped:
			m_Variants.emplace<KeyTypedEvent>(std::get<KeyTypedEvent>(variant));
			break;
		case KeyPressed:
			m_Variants.emplace<KeyPressedEvent>(std::get<KeyPressedEvent>(variant));
			break;
		case KeyReleased:
			m_Variants.emplace<KeyReleasedEvent>(std::get<KeyReleasedEvent>(variant));
			break;
		case MouseMoved:
			m_Variants.emplace<MouseMovedEvent>(std::get<MouseMovedEvent>(variant));
			break;
		case MouseScrolled:
			m_Variants.emplace<MouseScrolledEvent>(std::get<MouseScrolledEvent>(variant));
			break;
		case MouseButtonPressed:
			m_Variants.emplace<MouseButtonPressedEvent>(std::get<MouseButtonPressedEvent>(variant));
			break;
		case MouseButtonReleased:
			m_Variants.emplace<MouseButtonReleasedEvent>(std::get<MouseButtonReleasedEvent>(variant));
			break;
		case WindowClosed:
			m_Variants.emplace<WindowClosedEvent>(std::get<WindowClosedEvent>(variant));
			break;
		case WindowResized:
			m_Variants.emplace<WindowResizedEvent>(std::get<WindowResizedEvent>(variant));
			break;
		default:
			//MT_CORE_ASSERT(false, "invalid event type");
			break;
		}
		m_Type = type;
	}

	constexpr void operator=(const Event& e)
	{
		switch (e.m_Type)
		{
		case None:
			m_Variants.emplace<NoneEvent>(std::get<NoneEvent>(e.m_Variants));
			break;
		case KeyTyped:
			m_Variants.emplace<KeyTypedEvent>(std::get<KeyTypedEvent>(e.m_Variants));
			break;
		case KeyPressed:
			m_Variants.emplace<KeyPressedEvent>(std::get<KeyPressedEvent>(e.m_Variants));
			break;
		case KeyReleased:
			m_Variants.emplace<KeyReleasedEvent>(std::get<KeyReleasedEvent>(e.m_Variants));
			break;
		case MouseMoved:
			m_Variants.emplace<MouseMovedEvent>(std::get<MouseMovedEvent>(e.m_Variants));
			break;
		case MouseScrolled:
			m_Variants.emplace<MouseScrolledEvent>(std::get<MouseScrolledEvent>(e.m_Variants));
			break;
		case MouseButtonPressed:
			m_Variants.emplace<MouseButtonPressedEvent>(std::get<MouseButtonPressedEvent>(e.m_Variants));
			break;
		case MouseButtonReleased:
			m_Variants.emplace<MouseButtonReleasedEvent>(std::get<MouseButtonReleasedEvent>(e.m_Variants));
			break;
		case WindowClosed:
			m_Variants.emplace<WindowClosedEvent>(std::get<WindowClosedEvent>(e.m_Variants));
			break;
		case WindowResized:
			m_Variants.emplace<WindowResizedEvent>(std::get<WindowResizedEvent>(e.m_Variants));
			break;
		default:
			//MT_CORE_ASSERT(false, "invalid event type");
			break;
		}
		m_Type = e.m_Type;
	}

	bool IsHandled() const { return m_IsHandled; }

	constexpr std::string ToString()
		{
			switch (m_Type)
			{
			case KeyTyped:
				return "KeyTypedEvent";
			case KeyPressed:
				return "KeyPressedEvent";
			case KeyReleased:
				return "KeyReleasedEvent";
			case MouseMoved:
				return "MouseMovedEvent";
			case MouseScrolled:
				return "MouseScrolledEvent";
			case MouseButtonPressed:
				return "MouseButtonPressedEvent";
			case MouseButtonReleased:
				return "MouseButtonReleasedEvent";
			case WindowClosed:
				return "WindowClosedEvent";
			case WindowResized:
				return "WindowResizedEvent";
			default:
				return "Invalid Event type";

			}
		}

private:
	EventVariants m_Variants;
	EventType m_Type;
	bool m_IsHandled = false;

	friend class EventDispatcher;
};

class EventDispatcher
{
public:
	template <typename T>
	using EventFunction = std::function<bool(T&)>;

	EventDispatcher(Event& event)
	{
		m_Event = &event;
	}

	template <typename T>
	bool Dispatch(EventFunction<T> function)
	{
		if(T::GetStaticType() == m_Event->m_Type)
		{
			m_Event->m_IsHandled |= function(std::get<T>(m_Event->m_Variants));
			return true;
		}
		return false;
	}
private:
	Event* m_Event;
};

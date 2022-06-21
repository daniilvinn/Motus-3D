#pragma once

#include <Core/Events/Event.h>
#include <array>

namespace Motus3D
{
	class EventQueue
	{
	public:
		EventQueue();
		~EventQueue();

		uint32_t GetActualAmount() { return m_ActualAmount; }
		void Push(Event e);
		void Clear();

		Event& operator[](uint32_t index) { return m_Queue[index]; }

	private:
		std::array<Event, APP_CONFIG_MAX_EVENT_QUEUE_SIZE> m_Queue;
		uint32_t m_ActualAmount = 0;

	};
}


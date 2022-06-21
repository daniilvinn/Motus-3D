#include "EventQueue.h"

#include "Core/Logger.h"

namespace Motus3D
{
	EventQueue::EventQueue()
	{
		m_ActualAmount = 0;
	}

	EventQueue::~EventQueue()
	{

	}

	void EventQueue::Clear()
	{
		for(int i = 0; i < m_ActualAmount; i++)
		{
			m_Queue[i] = Event();
		}
		m_ActualAmount = 0;
	}

	void EventQueue::Push(Event event)
	{
		m_Queue[m_ActualAmount] = event;
		m_ActualAmount++;
	}
}

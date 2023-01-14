#pragma once

#include <Core/Macros.h>
#include <Core/Events/Event.h>

#include <utility>

#include <Visus.h>

namespace Motus3D
{
	// Interface for Application Window class. To be inherited by implementation class in Platform/ directory, e.g. "WinApplicationWindow"
	class MOTUS_API ApplicationWindow
	{
	public:
		using EventHandlerFn = std::function<void(Event)>;
		virtual ~ApplicationWindow() {};

		virtual void SetEventHandler(const EventHandlerFn& fn) = 0;
		virtual void OnUpdate() = 0;

		virtual void* GetHandle() = 0;
		virtual std::pair<uint32_t, uint32_t> GetExtent() = 0; // width, height

	};
}


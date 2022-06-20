#pragma once

#include <Core/Macros.h>
#include <Core/Events/Event.h>
#include <utility>

namespace Motus3D
{
	// Interface for Application Window class. To be inherited by implementation class in Platform/ directory, for instance "WinApplicationWindow"
	class ApplicationWindow
	{
	public:
		using EventHandlerFn = std::function<void(Event*)>;
		virtual ~ApplicationWindow() {};

		virtual void SetEventHandle(const EventHandlerFn& fn) = 0;

		virtual void* GetHandle() = 0;
		virtual std::pair<uint32_t, uint32_t> GetExtent() = 0; // width, height

		virtual void OnUpdate() = 0;


	};
}


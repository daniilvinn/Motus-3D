#pragma once

#include <Core/Macros.h>
#include <Core/Events/Event.h>

namespace Motus3D
{
	// Layer class interface. To be inherited to implement virtual methods.
	class MOTUS_API Layer
	{
	public:

		virtual void OnUpdate() = 0;
		virtual void OnEvent(Event& e) = 0;
		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

	};
}
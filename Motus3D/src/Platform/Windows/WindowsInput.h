#pragma once

#include <Core/Input.h>

namespace Motus3D {

	class WindowsInput : public Input
	{
	protected:
		bool KeyPressed_Impl(KeyCode code) override;

		bool MouseButtonPressed_Impl(KeyCode code) override;

		std::pair<int32_t, int32_t> MousePosition_Impl() override;

		float MouseScrolledY_Impl() override;

		float MouseScrolledX_Impl() override;

	};

}
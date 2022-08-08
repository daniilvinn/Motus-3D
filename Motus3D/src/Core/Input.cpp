#include "Input.h"

#include <Platform/Windows/WindowsInput.h>

namespace Motus3D {

#ifdef MT_PLATFORM_WINDOWS
	Input* Input::s_Instance = new WindowsInput;
#else
	#error Unknown platform (expected WIN32)
#endif
}
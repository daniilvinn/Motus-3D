#pragma once

#include <Core/Macros.h>

#ifdef MT_SCENE_API
	#undef MT_SCENE_API
	#define MT_SCENE_API __declspec(dllexport)
#else
	#define MT_SCENE_API __declspec(dllimport)
#endif
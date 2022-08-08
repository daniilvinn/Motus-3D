#pragma once

// TODO: to be improved
#ifdef MT_PLATFORM_WINDOWS
	#ifdef MT_DYNAMIC
		#ifdef MT_BUILD_DLL
			#define MOTUS_API __declspec(dllexport)
		#else
			#define MOTUS_API __declspec(dllimport)
		#endif
	#elif defined(MT_STATIC)
		#define MOTUS_API
	#elif defined(MT_STATIC) && defined (MT_DYNAMIC)
		#error MOTUS: can't be built both dynamically and statically! Check your premake file!
	#endif
#endif

#ifdef MT_DEBUG
	#ifndef MT_ASSERTS_ENABLED
		#define MT_ASSERTS_ENABLED
		#ifdef MT_PLATFORM_WINDOWS
			#define MT_CORE_ASSERT(expression, ...) if(!(expression)){\
														MT_CORE_LOG_ERROR("Assertion failed: {}",__VA_ARGS__); __debugbreak();}\
														else{}
			#define MT_ASSERT(expression, ...)		if(!(expression)){\
														MT_LOG_ERROR("Assertion failed: {}",__VA_ARGS__); __debugbreak();}\
														else{}
		#endif
	#endif
#else
	#define MT_CORE_ASSERT(expression, ...)
	#define MT_ASSERT(expression, ...)
#endif

#include "Memory.h"

#define BIT(bit) (1 << bit)

// Thanks Yan TheCherno Chernikov for this macro <3
// https://github.com/TheCherno
#define MT_BIND_EVENT_FUNCTION(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
// --------------------------------------

// TODO: move this to application's config struct
#define APP_CONFIG_MAX_EVENT_QUEUE_SIZE 200
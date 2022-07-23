#pragma once

#include <Core/Macros.h>

#ifdef VISUS_API
	#undef VISUS_API
	#define VISUS_API __declspec(dllexport)
#else
	#define VISUS_API __declspec(dllimport)
#endif

#ifdef VISUS_DEBUG
	#define VISUS_INTERNAL_ENABLE_VALIDATION 1
#else
	#define VISUS_INTERNAL_ENABLE_VALIDATION 0
#endif

#include <Visus/Core/Logger.h>

#ifdef VISUS_DEBUG
#define VK_CHECK_RESULT(expression) if(expression != VK_SUCCESS) {\
										VISUS_ERROR("Vulkan call failed: {0} || Location: {1} ({2})", #expression, __FILE__, __LINE__); \
									} else {}

#define VISUS_ASSERT(expression, ...) if(!(expression)){\
										VISUS_ERROR("Assertion failed: {}", __VA_ARGS__); __debugbreak();}\
									  else{}

#else 
	#define VK_CHECK_RESULT(expression) expression
	#define VISUS_ASSERT(expression, ...)
#endif
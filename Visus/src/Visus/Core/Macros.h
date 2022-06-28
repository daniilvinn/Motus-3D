#pragma once

#ifdef VISUS_API
	#undef VISUS_API
	#define VISUS_API __declspec(dllexport)
#else
	#define VISUS_API __declspec(dllimport)
#endif

#ifdef VISUS_DEBUG
	#define VISUS_INTERNAL_ENABLE_VALIDATION_LAYERS 1
#else
	#define VISUS_INTERNAL_ENABLE_VALIDATION_LAYERS 0
#endif


#include <memory>
namespace Visus
{
	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include <Visus/Core/Logger.h>

#ifdef VISUS_DEBUG
#define VK_CHECK_RESULT(expression) if(expression != VK_SUCCESS) {\
										VISUS_ERROR("Vulkan call failed: {0} || Location: {1} ({2})", #expression, __FILE__, __LINE__); \
									} else {}

#else 
	#define VK_CHECK_RESULT(expression) expression
#endif
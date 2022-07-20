#pragma once

#include <memory>

#ifdef MT_DEBUG
	#define MT_CHECK_CLASS_COMPATIBILITY(t1, t2) static_assert(std::is_base_of<t1, t2>().value || std::is_base_of<t2, t1>().value, "Impossible to cast!");
#else
	#define MT_CHECK_CLASS_COMPATIBILITY(t1, t2)
#endif

namespace Motus3D
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

	template <typename T1, typename T2>
	Ref<T1> RefAs(Ref<T2>& ptr)
	{
		// Macro below may not be working. To be tested.
		MT_CHECK_CLASS_COMPATIBILITY(T1, T2);
		return std::static_pointer_cast<T1>(ptr);
	};

}
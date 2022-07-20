#pragma once

#include <Visus/Core/Macros.h>
#include <spdlog/spdlog.h>

namespace Motus3D
{
	class Logger
	{
	public:
		static void Init();
		static void Shutdown();
		static Ref<spdlog::logger> Get() { return s_Logger; };

	private:
		static Ref<spdlog::logger> s_Logger;
	};

}

#ifdef VISUS_DEBUG
	#define VISUS_TRACE(...)	Motus3D::Logger::Get()->trace(__VA_ARGS__);
	#define VISUS_INFO(...)		Motus3D::Logger::Get()->info(__VA_ARGS__);
	#define VISUS_WARN(...)		Motus3D::Logger::Get()->warn(__VA_ARGS__);
	#define VISUS_ERROR(...)	Motus3D::Logger::Get()->error(__VA_ARGS__);
#else
	#define VISUS_TRACE(...)
	#define VISUS_INFO(...)
	#define VISUS_WARN(...)
	#define VISUS_ERROR(...)
#endif

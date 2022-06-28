#pragma once

#include <Visus/Core/Macros.h>
#include <spdlog/spdlog.h>

namespace Visus
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
	#define VISUS_TRACE(...)	Visus::Logger::Get()->trace(__VA_ARGS__);
	#define VISUS_INFO(...)		Visus::Logger::Get()->info(__VA_ARGS__);
	#define VISUS_WARN(...)		Visus::Logger::Get()->warn(__VA_ARGS__);
	#define VISUS_ERROR(...)	Visus::Logger::Get()->error(__VA_ARGS__);
#else
	#define VISUS_TRACE(...)
	#define VISUS_INFO(...)
	#define VISUS_WARN(...)
	#define VISUS_ERROR(...)
#endif

#pragma once

#include <Core/Macros.h>
#include <spdlog/spdlog.h>

namespace Motus3D
{
#ifdef MT_BUILD_DLL
	class CoreLogger
	{
	public:
		static void Init();
		static void Shutdown(){};

		static Ref<spdlog::logger>& Get() { return s_Logger; }
	private:
		static Ref<spdlog::logger> s_Logger;
	};
#endif

	class MOTUS_API ClientLogger
	{
	public:
		static void Init();
		static void Shutdown(){};

		static Ref<spdlog::logger>& Get() { return s_Logger; }
	private:
		static Ref<spdlog::logger> s_Logger;
	};
}

#ifdef MT_DEBUG
	#ifdef MT_BUILD_DLL
		#define MT_CORE_LOG_TRACE(...)	Motus3D::CoreLogger::Get()->trace(__VA_ARGS__);
		#define MT_CORE_LOG_INFO(...)	Motus3D::CoreLogger::Get()->info(__VA_ARGS__);
		#define MT_CORE_LOG_WARN(...)	Motus3D::CoreLogger::Get()->warn(__VA_ARGS__);
		#define MT_CORE_LOG_ERROR(...)	Motus3D::CoreLogger::Get()->error(__VA_ARGS__);
		#define MT_CORE_LOG_FATAL(...)	Motus3D::CoreLogger::Get()->critical(__VA_ARGS__);
	#endif

	#define MT_LOG_TRACE(...)		Motus3D::ClientLogger::Get()->trace(__VA_ARGS__);
	#define MT_LOG_INFO(...)		Motus3D::ClientLogger::Get()->info(__VA_ARGS__);
	#define MT_LOG_WARN(...)		Motus3D::ClientLogger::Get()->warn(__VA_ARGS__);
	#define MT_LOG_ERROR(...)		Motus3D::ClientLogger::Get()->error(__VA_ARGS__);
	#define MT_LOG_FATAL(...)		Motus3D::ClientLogger::Get()->critical(__VA_ARGS__);
#else
	#define MT_CORE_LOG_TRACE(...)
	#define MT_CORE_LOG_INFO(...)
	#define MT_CORE_LOG_WARNING(...)
	#define MT_CORE_LOG_ERROR(...)
	#define MT_CORE_LOG_FATAL(...)

	#define MT_LOG_TRACE(...)
	#define MT_LOG_INFO(...)
	#define MT_LOG_WARNING(...)
	#define MT_LOG_ERROR(...)
	#define MT_LOG_FATAL(...)
#endif

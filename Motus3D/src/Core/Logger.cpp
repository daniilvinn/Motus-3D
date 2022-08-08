#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>

#include <Visus.h>

namespace Motus3D
{
	Ref<spdlog::logger> CoreLogger::s_Logger;
	Ref<spdlog::logger> ClientLogger::s_Logger;

	// ===========
	// Core Logger
	// ===========
	void CoreLogger::Init()
	{
		// TODO: think about using default loggers instead of async ones
		spdlog::set_pattern("%^[%T][%n]: %v%$");

		// TODO: fix async loggers
		s_Logger = spdlog::stdout_color_mt("Motus3D");;
		s_Logger->set_level(spdlog::level::trace);

	}

	// =============
	// Client Logger
	// =============
	void ClientLogger::Init()
	{
		spdlog::set_pattern("%^[%T][%n]: %v%$");
		s_Logger = spdlog::stdout_color_mt("Client");
		s_Logger->set_level(spdlog::level::trace);
	}


}

#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>

namespace Motus3D
{
	Ref<spdlog::logger> Logger::s_Logger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T][%n]: %v%$");
		s_Logger = spdlog::stdout_color_mt("Motus3D");
		s_Logger->set_level(spdlog::level::trace);

	}
}

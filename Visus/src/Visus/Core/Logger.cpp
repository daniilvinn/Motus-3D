#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>

namespace Visus
{
	Ref<spdlog::logger> Logger::s_Logger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T][%n]: %v%$");
		s_Logger = spdlog::stdout_color_mt("Visus");
		s_Logger->set_level(spdlog::level::trace);
	}
}

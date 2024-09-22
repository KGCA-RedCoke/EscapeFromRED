#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

void Logger::Initialize()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");

	// Set Engine Logger
	CoreLogger = spdlog::stdout_color_mt("Jacob Engine");
	CoreLogger->set_level(spdlog::level::trace);
}
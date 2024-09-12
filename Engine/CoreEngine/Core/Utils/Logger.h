#pragma once
#include <spdlog/spdlog.h>

#include "Core/Manager/Manager_Base.h"

#define G_Logger Logger::Get()

class Logger : public TSingleton<Logger>
{
public:
	void Initialize();

	inline std::shared_ptr<spdlog::logger>& GetCoreLogger()
	{
		return CoreLogger;
	}

private:
	std::shared_ptr<spdlog::logger> CoreLogger;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<Logger>;

	Logger()  = default;
	~Logger() = default;

public:
	Logger(const Logger&)            = delete;
	Logger& operator=(const Logger&) = delete;

#pragma endregion
};

#define LOG_CORE_TRACE(...)	  G_Logger.GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)     G_Logger.GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)     G_Logger.GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)    G_Logger.GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...)    G_Logger.GetCoreLogger()->critical(__VA_ARGS__)

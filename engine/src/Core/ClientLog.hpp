#pragma once 

#include "defines.hpp"

#include "LogLevel.hpp"

RPR_API void log_output(Log_Level level, const char* message, ...);

#define RPR_CLIENT_CRITICAL(message, ...) log_output(LOG_LEVEL_CRITICAL, message, __VA_ARGS__);
#define RPR_CLIENT_ERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, __VA_ARGS__);
#define RPR_CLIENT_WARN(message, ...) log_output(LOG_LEVEL_WARN, message, __VA_ARGS__);
#define RPR_CLIENT_INFO(message, ...) log_output(LOG_LEVEL_INFO, message, __VA_ARGS__);
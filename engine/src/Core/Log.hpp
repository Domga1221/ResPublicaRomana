#pragma once 

#include "defines.hpp"

enum Log_Level {
    LOG_LEVEL_CRITICAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,

    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5,
};

b8 Log_Initialize();
void Log_Shutdown();

// -- 
#include <string>
template <typename T>
const T& log_helper(const T& t) {
    return t;
}

const char* log_helper(std::string& s);

void log_output(Log_Level level, const char* message, ...);
template<typename... Args> 
RPR_API void log(Log_Level level, const char* message, Args&&... args) {
    log_output(level, message, log_helper(args)...);
}
// --

#define RPR_CRITICAL(message, ...) log(LOG_LEVEL_CRITICAL, message, __VA_ARGS__);
#define RPR_ERROR(message, ...) log(LOG_LEVEL_ERROR, message, __VA_ARGS__);
#define RPR_WARN(message, ...) log(LOG_LEVEL_WARN, message, __VA_ARGS__);
#define RPR_INFO(message, ...) log(LOG_LEVEL_INFO, message, __VA_ARGS__);

#if RPR_RELEASE == 1
#define RPR_DEBUG 
#define RPR_TRACE 
#else 
#define RPR_DEBUG(message, ...) log(LOG_LEVEL_DEBUG, message, __VA_ARGS__);
#define RPR_TRACE(message, ...) log(LOG_LEVEL_TRACE, message, __VA_ARGS__);
#endif 
#include "Log.hpp"

#include <cstdarg>

#include "Platform/Platform.hpp"


// TODO: Add time
void log_output(Log_Level level, const char* message, ...) {
    const char* levelStrings[6] = {"[CRITICAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};
    b8 error = level < LOG_LEVEL_WARN;

    const i32 maxMessageLength = 1000;
    char outMessage[maxMessageLength];
    memset(outMessage, 0, maxMessageLength);

    va_list argList;
    va_start(argList, message);
    vsnprintf(outMessage, maxMessageLength, message, argList);
    va_end(argList);

    char outMessage2[maxMessageLength];
    sprintf(outMessage2, "%s%s\n", levelStrings[level], outMessage);
    
    if(error) 
        Platform_ConsoleLog(outMessage2, level);
    else 
        Platform_ConsoleLogError(outMessage2, level);
}

const char* log_helper(std::string& s) {
    return s.c_str();
}
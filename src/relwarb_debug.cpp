#include "relwarb_debug.h"

#include <stdarg.h>
#include <stdio.h>

#include "relwarb_defines.h"

internal const char* GetLogLevelMessage(LogLevel level)
{
    switch (level)
    {
        case Log_Debug: return "Debug: ";
        case Log_Info:  return "Info: ";
        case Log_Error: return "Error: ";
        case Log_Warning: return "Warning: ";
    }

    return "";
}

internal void VarLog(LogLevel level, const char* format, va_list args)
{
    local_persist FILE* logFile = fopen("log.txt", "w");

    char str[1024];
    vsnprintf(str, 1024, format, args);

#if 1
#ifdef OS_WINDOWS
    OutputDebugString(GetLogLevelMessage(level));
    OutputDebugString(str);
    OutputDebugString("\n");
#else
    fprintf(stderr, "%s", GetLogLevelMessage(level));
    fprintf(stderr, str);
    fprintf(stderr, "\n");
#endif
#else
    fprintf(logFile, "%s%s\n", GetLogLevelMessage(level), str);
#endif
}

void Log(LogLevel level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    VarLog(level, format, args);
    va_end(args);
}

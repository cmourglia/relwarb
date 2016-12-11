#ifndef RELWARB_DEBUG_H
#define RELWARB_DEBUG_H

enum LogLevel
{
    Log_Debug,
    Log_Error,
    Log_Info,
    Log_Warning,
};

void Log(LogLevel level, const char* format, ...);

#endif
#include "Log.h"
#include <cstdio>
#include <cstring>
#include <cstdarg>
bool Log::m_Inited = false;
unsigned short Log::m_LogLevel = 0;

Log::~Log()
{
    //dtor
}

void Log::Init()
{
    m_Inited = true;
    m_LogLevel = 0;

}

void Log::SetFilter(int logLevel)
{

    Log::m_LogLevel = logLevel;
}

void Log::Debug(const char* fmt, ...)
{
    if(Log::m_LogLevel > 0) return;

    va_list args;
    printf(m_Format[0],"");  //3 times print.... should not.
    va_start(args,fmt);

    vprintf(fmt,args);
    va_end(args);
    printf("\n");
}

void Log::LogOut(const char* fmt, ...)
{
    if(Log::m_LogLevel > 1) return;
    va_list args;
    printf(m_Format[1],"");
    va_start(args,fmt);
    vprintf(fmt,args);
    va_end(args);
    printf("\n");
}

void Log::Warning(const char* fmt, ...)
{
    if(Log::m_LogLevel > 2) return;
    va_list args;
    printf(m_Format[2],"");
    va_start(args,fmt);
    vprintf(fmt,args);
    va_end(args);
    printf("\n");
}

void Log::Error(const char* fmt, ...)
{
    if(Log::m_LogLevel > 3) return;
    va_list args;
    printf(m_Format[3],"");
    va_start(args,fmt);
    vprintf(fmt,args);
    va_end(args);
    printf("\n");
}

void Log::Fatal(const char* fmt, ...)
{
    va_list args;
    printf(m_Format[4],"");
    va_start(args,fmt);
    vprintf(fmt,args);
    va_end(args);
    printf("\n");
}

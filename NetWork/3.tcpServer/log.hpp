#pragma once

#include <iostream>
#include <string>
#include <stdarg.h>
#include <cstdio>
#include <ctime>

#define LOG_NORMAL "log.txt"
#define LOG_ERR "err.txt"

enum LOGLEVEL
{
    DEBUG,
    NORMAL,
    WARNING,
    ERROR,
    FATAL
};

const char* to_levelstr(int level)
{
    switch (level)
    {
    case DEBUG:return "DEBUG";
    case NORMAL:return "NORMAL";
    case WARNING:return "WARNING";
    case ERROR:return "ERROR";
    case FATAL:return "FATAL";
    default: return nullptr;
    }
}

void logMessage(int level, const char* format, ...)
{
    //[日志等级][时间戳/时间][pid][message]
    //[WARNING][2024-05-15 19:41:55][123][创建socket失败]
#define NUM 1024
    char logprefix[NUM];
    time_t now = time(nullptr);
    struct tm* time_now;
    time_now = localtime(&now);
    snprintf(logprefix, sizeof(logprefix), "[%s][%d-%d-%d %d:%d:%d][%d]",
        to_levelstr(level), time_now->tm_year+1900, time_now->tm_mon+1, time_now->tm_mday, time_now->tm_hour, time_now->tm_min, time_now->tm_sec, getpid());

    char logcontent[NUM];
    va_list arg;
    va_start(arg, format);
    vsnprintf(logcontent, sizeof(logcontent), format, arg);

    //std::cout << logprefix << logcontent << std::endl;

    FILE* log = fopen(LOG_NORMAL, "a"); 
    FILE* err = fopen(LOG_ERR, "a");
    if (log != nullptr && err != nullptr)
    {
        FILE* curr = nullptr;
        if (level == DEBUG || level == NORMAL || level == WARNING) curr = log;
        if (level == ERROR || level == FATAL) curr = err;
        if (curr) fprintf(curr, "%s%s\n", logprefix, logcontent);

        fclose(log);
        fclose(err);
    }
}

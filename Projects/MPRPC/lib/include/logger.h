#pragma once

#include "blockqueue.h"

enum LogLevel
{
    INFO,   // 普通信息
    ERROR,  // 错误信息
};

// MPRPC框架提供的日志系统
class Logger
{
public:
    // 获取日志的单例
    static Logger& GetInstanse();
    // 设置日志级别
    void SetLogLevel(LogLevel level);
    // 写日志
    void Log(std::string msg);
private:
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
private:
    int m_loglevel; // 记录日志级别
    BlockQueue<std::string> m_queue;    // 日志缓冲队列
};

// 定义宏
#define LOG_INFO(logmsgformat, ...) \
    do  \
    {   \
        Logger& logger = Logger::GetInstanse();\
        logger.SetLogLevel(INFO);\
        char c[1024] = {0};\
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__);\
        logger.Log(c);\
    } while ( 0 );
    
#define LOG_ERROR(logmsgformat, ...) \
    do  \
    {   \
        Logger& logger = Logger::GetInstanse();\
        logger.SetLogLevel(ERROR);\
        char c[1024] = {0};\
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__);\
        logger.Log(c);\
    } while ( 0 );

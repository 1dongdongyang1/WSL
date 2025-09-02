#include "Logger.h"
#include "Timestamp.h"

#include <iostream>

// 获取日志唯一单例对象
Logger& Logger::instance()
{
    static Logger logger;   // 线程安全
    return logger;
}

// 设置日志等级
void Logger::setLogLevel(int level)
{
    logLevel_ = level;
}

// 写日志   [日志等级]time : msg
void Logger::log(std::string msg)
{
    switch (logLevel_)
    {
    case INFO:
        std::cout << "[INFO]";
        break;
    case ERROR:
        std::cout << "[ERROR]";
        break;
    case FATAL:
        std::cout << "[FATAL]";
        break;
    case DEBUG:
        std::cout << "[DEBUG]";
        break;
    default:
        break;
    }

    // 输出time : msg
    std::cout << Timestamp::now().toString() << " : " << msg << std::endl;
}
#pragma once
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

class AppLogger {
public:
    static void init(const std::string& file = "/home/ddy/Projects/ChatServer/logs/app.log") {
        logger_ = spdlog::rotating_logger_mt(
            "app_logger", file, 1024 * 1024 * 5, 3); // 5MB * 3个文件
        spdlog::set_default_logger(logger_);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
        spdlog::set_level(spdlog::level::debug);  // 输出所有级别
    }

    static std::shared_ptr<spdlog::logger>& getLogger() { return logger_; }

private:
    static std::shared_ptr<spdlog::logger> logger_;
};


#define APP_LOG_TRACE(fmt, ...) spdlog::trace(fmt, ##__VA_ARGS__)
#define APP_LOG_DEBUG(fmt, ...) spdlog::debug(fmt, ##__VA_ARGS__)
#define APP_LOG_INFO(fmt, ...)  spdlog::info(fmt, ##__VA_ARGS__)
#define APP_LOG_WARN(fmt, ...)  spdlog::warn(fmt, ##__VA_ARGS__)
#define APP_LOG_ERROR(fmt, ...) spdlog::error(fmt, ##__VA_ARGS__)
#define APP_LOG_CRITICAL(fmt, ...) spdlog::critical(fmt, ##__VA_ARGS__)
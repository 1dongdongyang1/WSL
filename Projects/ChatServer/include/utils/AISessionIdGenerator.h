#pragma once

#include <string>

/// @brief AI会话ID生成器
class AISessionIdGenerator {
public:
    AISessionIdGenerator();
    std::string generate();
};
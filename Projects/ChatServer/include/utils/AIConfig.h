#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "http/utils/JsonUtil.h"

struct AITool {
    std::string name;
    std::string description;
    std::unordered_map<std::string, std::string> parameters; // 参数名 -> 参数值
};

struct AIToolCall {
    std::string toolName;
    json arguments;
    bool isToolCall = false; // 是否为工具调用
};

class AIConfig {
public:
    bool loadFromFile(const std::string& filePath);
    std::string buildPrompt(const std::string& userQuestion) const;
    AIToolCall parseAIResponse(const std::string& response) const;
    std::string buildToolResultPrompt(const std::string& userQuestion, const std::string& toolName, const json& toolArguments, const json& toolResult) const;

private:
    std::string buildToolList() const;

private:
    std::string promptTemplate_;
    std::vector<AITool> tools_;
};
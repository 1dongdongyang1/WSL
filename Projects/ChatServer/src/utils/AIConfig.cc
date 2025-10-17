#include "utils/AIConfig.h"
#include "utils/LogUtil.h"

#include <fstream>
#include <ostream>
#include <regex>

bool AIConfig::loadFromFile(const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        return false;
    }

    json configJson;
    inFile >> configJson;

    if (!configJson.contains("prompt_template") || !configJson["prompt_template"].is_string()) {
        APP_LOG_ERROR("Invalid or missing 'prompt_template' in config");
        return false;
    }
    promptTemplate_ = configJson.value("prompt_template", "");

    if (configJson.contains("tools") && configJson["tools"].is_array()) {
        for (const auto& toolJson : configJson["tools"]) {
            AITool tool;
            tool.name = toolJson.value("name", "");
            tool.description = toolJson.value("desc", "");
            if (toolJson.contains("params") && toolJson["params"].is_object()) {
                for (auto& [key, value] : toolJson["params"].items()) {
                    tool.parameters[key] = value.get<std::string>();
                }
            }
            tools_.push_back(std::move(tool));
        }
    }

    return true;
}

std::string AIConfig::buildToolList() const {
    std::ostringstream toolListStream;
    for (const auto& tool : tools_) {
        toolListStream << tool.name << "(";
        bool first = true;
        for (const auto& [paramName, paramDesc] : tool.parameters) {
            if (!first) {
                toolListStream << ", ";
            }
            toolListStream << paramName << ": " << paramDesc;
            first = false;
        }
        toolListStream << "): " << tool.description << "\n";
    }
    return toolListStream.str();
}

std::string AIConfig::buildPrompt(const std::string& userQuestion) const {
    std::string result = promptTemplate_;
    result = std::regex_replace(result, std::regex("\\{tool_list\\}"), buildToolList());
    result = std::regex_replace(result, std::regex("\\{user_question\\}"), userQuestion);
    return result;
}

AIToolCall AIConfig::parseAIResponse(const std::string& response) const {
    AIToolCall toolCall;
    try {
        // Try parsing as JSON
        json responseContent = json::parse(response);
        if (responseContent.contains("tool") && responseContent["tool"].is_string()) {
            toolCall.toolName = responseContent["tool"].get<std::string>();
            if (responseContent.contains("args") && responseContent["args"].is_object()) {
                toolCall.arguments = responseContent["args"];
            }
            toolCall.isToolCall = true;
        }
    }
    catch (const std::exception& e) {
        // Parsing failed, treat as normal answer
        toolCall.isToolCall = false;
    }
    return toolCall;
}

std::string AIConfig::buildToolResultPrompt(const std::string& userQuestion, const std::string& toolName, const json& toolArguments, const json& toolResult) const {
    std::ostringstream promptStream;
    promptStream << "下面是用户的问题: " << userQuestion << "\n"
        << "我刚才调用了工具[" << toolName << "], 参数为:"
        << toolArguments.dump() << "\n"
        << "工具返回的结果是: " << toolResult.dump() << "\n"
        << "请基于以上信息，用自然语言给出最终的回答。";
    return promptStream.str();
}
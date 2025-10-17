#pragma once

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "utils/AIConfig.h"
#include "utils/AIFactory.h"
#include "utils/AIToolRegistry.h"

#include "http/utils/JsonUtil.h"

/**
 * Agent = 大脑 + 记忆 + 工具
 * 大脑：AI模型           -> 调用外部API        -> AIStrategy + AIFactory
 * 记忆：聊天记录         -> messages_         
 * 工具：调用外部API的能力 -> 协议:MCP
 *
 * AIHelper -> RabbitMQ -> 大模型
 *
 */

class AIHelper {
public:
    AIHelper();

    void setStrategy(std::shared_ptr<AIStrategy> strategy);
    void addMessage(int userId, const std::string& userName, bool isUser, const std::string& content, const std::string& sessionId);
    std::string chat(int userId, const std::string& userName, const std::string& sessionId, const std::string& question, const std::string& modelType);

    // Getters
    const std::vector<std::pair<std::string, long long>>& getMessages() const {
        return messages_;
    }

private:
    std::string escapeString(const std::string& str);
    void pushMessageToDB(int userId, const std::string& userName, bool isUser, const std::string& content, const std::string& sessionId, long long timestamp);
    json executeRequest(const json& request);

private:
    std::shared_ptr<AIStrategy> strategy_;
    std::vector<std::pair<std::string, long long>> messages_; // 聊天记录 string + 时间戳
};
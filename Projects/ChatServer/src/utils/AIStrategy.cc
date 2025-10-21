#include "utils/AIStrategy.h"
#include "utils/AIFactory.h"

std::string AliyunStrategy::getApiUrl() const {
    return "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions";
}

std::string AliyunStrategy::getApiKey() const {
    return apiKey_;
}
std::string AliyunStrategy::getModel() const {
    return "qwen-plus";
}

json AliyunStrategy::buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const {
    json request;
    request["model"] = getModel();
    json msgArray = json::array();
    for(size_t i = 0; i < messages.size(); ++i) {
        json msg;
        msg["role"] = (i % 2 == 0) ? "user" : "assistant";
        msg["content"] = messages[i].first;
        msgArray.push_back(msg);
    }
    request["messages"] = msgArray;
    return request;
}

std::string AliyunStrategy::parseResponse(const json& response) const {
    if(response.contains("choices") && !response["choices"].empty()) {
        return response["choices"][0]["message"]["content"].get<std::string>();
    }
    return "";
}

static StrategyRegister<AliyunStrategy> registerAliyunStrategy("Aliyun");
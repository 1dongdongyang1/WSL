#include "utils/AIHelper.h"
#include "utils/MQManager.h"
#include "utils/LogUtil.h"

#include <curl/curl.h>


// Default strategy is Aliyun
AIHelper::AIHelper() {
    strategy_ = StrategyFactory::instance().createStrategy("Aliyun");
}

void AIHelper::setStrategy(std::shared_ptr<AIStrategy> strategy) {
    strategy_ = strategy;
}

// save message to memory and DB
void AIHelper::addMessage(int userId, const std::string& userName, bool isUser, const std::string& content, const std::string& sessionId) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    long long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    messages_.emplace_back(content, timestamp);
    // asynchronously push to DB
    pushMessageToDB(userId, userName, isUser, content, sessionId, timestamp);
}

void AIHelper::pushMessageToDB(int userId, const std::string& userName, bool isUser, const std::string& content, const std::string& sessionId, long long timestamp) {
    std::string safeUserName = escapeString(userName);
    std::string safeContent = escapeString(content);
    std::string sql = "INSERT INTO chat_messages (user_id, user_name, is_user, content, session_id, timestamp) VALUES ("
                      + std::to_string(userId) + ", '"
                      + safeUserName + "', "
                      + (isUser ? "1" : "0") + ", '"
                      + safeContent + "', '"
                      + sessionId + "', "
                      + std::to_string(timestamp) + ");";

    // Execute SQL asynchronously
    MQManager::instance().publish("db_queue", sql);    
}

std::string AIHelper::escapeString(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '\'': escaped += "\\'"; break;
            case '\"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

std::string AIHelper::chat(int userId, const std::string& userName, const std::string& sessionId, const std::string& question, const std::string& modelType) {
    // set strategy based on modelType
    setStrategy(StrategyFactory::instance().createStrategy(modelType));
    addMessage(userId, userName, true, question, sessionId);

    if (false == strategy_->isMCPModel) {
        json request = strategy_->buildRequest(messages_);
        json response = executeRequest(request);
        std::string answer = strategy_->parseResponse(response);
        addMessage(userId, userName, false, answer, sessionId);
        return answer.empty() ? "[Error] cannot get answer from model" : answer;
    }

    // support MCP model
    AIConfig config;
    config.loadFromFile("/home/ddy/Projects/ChatServer/resource/config.json");
    std::string prompt = config.buildPrompt(question);
    APP_LOG_INFO("ai prompt: {}", prompt);

    messages_.emplace_back(prompt, 0);  // prompt don't need save to DB
    json request = strategy_->buildRequest(messages_);
    messages_.pop_back();               // remove prompt
    json response = executeRequest(request);
    std::string answer = strategy_->parseResponse(response);

    APP_LOG_INFO("ai first answer: {}", answer);
    AIToolCall call = config.parseAIResponse(answer);

    // 1. AI don't want to call tool, just answer
    if(!call.isToolCall) {
        addMessage(userId, userName, false, answer, sessionId);
        return answer;
    }

    // 2. AI want to call tool  -> call tool -> get tool result -> push tool result to AI -> get final answer
    json toolResult;
    AIToolRegistry registry;
    try {
        toolResult = registry.callTool(call.toolName, call.arguments);
        APP_LOG_INFO("call tool {} success, result: {}", call.toolName, toolResult.dump());
    }
    catch (const std::exception& e) {
        std::string errorMsg = std::string("[Error] call tool ") + call.toolName + " failed: " + e.what();
        addMessage(userId, userName, false, errorMsg, sessionId);
        APP_LOG_ERROR("call tool {} failed: {}", call.toolName, e.what());
        return errorMsg;
    }

    std::string secondPrompt = config.buildToolResultPrompt(question, call.toolName, call.arguments, toolResult);
    APP_LOG_INFO("ai second prompt: {}", secondPrompt);
    messages_.emplace_back(secondPrompt, 0); 

    json secondRequest = strategy_->buildRequest(messages_);
    messages_.pop_back();               // remove second prompt
    json secondResponse = executeRequest(secondRequest);
    std::string finalAnswer = strategy_->parseResponse(secondResponse);
    addMessage(userId, userName, false, finalAnswer, sessionId);
    return finalAnswer.empty() ? "[Error] cannot get final answer from model" : finalAnswer;
}

json AIHelper::executeRequest(const json& request) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string apiUrl = strategy_->getApiUrl();
    std::string apiKey = strategy_->getApiKey();
    APP_LOG_INFO("APIURL: {}, APIKey: {}", apiUrl, apiKey);

    // request headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());

    std::string requestData = request.dump();
    std::string responseData;

    curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
        size_t totalSize = size * nmemb;
        std::string* str = static_cast<std::string*>(userp);
        str->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    });

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("CURL request failed: ") + curl_easy_strerror(res));
    }

    try {
        return json::parse(responseData);
    }
    catch (const json::parse_error& e) {
        throw std::runtime_error(std::string("Failed to parse JSON response: ") + e.what());
    }
}
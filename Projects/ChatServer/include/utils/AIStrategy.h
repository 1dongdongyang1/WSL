#pragma once

#include <string>
#include <memory>
#include <vector>
#include <utility>  // for std::pair

#include "http/utils/JsonUtil.h"

/// @brief 策略模式封装多个模型对象
class AIStrategy {
public:
    virtual ~AIStrategy() = default;
    virtual std::string getApiUrl() const = 0;
    virtual std::string getApiKey() const = 0;
    virtual std::string getModel() const = 0;
    virtual json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const = 0;
    virtual std::string parseResponse(const json& response) const = 0;

    bool isMCPModel = false;
};

class AliyunStrategy : public AIStrategy {
public:
    AliyunStrategy();
    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;
    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;
private:
    std::string apiKey_;
};

class DouBaoStrategy : public AIStrategy {
public:
    DouBaoStrategy();
    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;
    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;
private:
    std::string apiKey_;
};

class AliyunMCPStrategy : public AIStrategy {
public:
    AliyunMCPStrategy();
    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;
    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;
private:
    std::string apiKey_;
};

class AliyunRAGStrategy : public AIStrategy {
public:
    AliyunRAGStrategy();
    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;
    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;
private:
    std::string apiKey_;
};
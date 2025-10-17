#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include "http/utils/JsonUtil.h"

class AIToolRegistry {
public:
    using ToolFunc = std::function<json(const json& args)>;

    AIToolRegistry();

    void registerTool(const std::string& name, ToolFunc func);
    bool hasTool(const std::string& name) const;
    json callTool(const std::string& name, const json& args) const;

private:
    static json getWeather(const json& args);
    static json getTime(const json& args);

private:
    std::unordered_map<std::string, ToolFunc> tools_;
};
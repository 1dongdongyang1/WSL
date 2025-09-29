#pragma once

#include <regex>    // 正则表达式
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

#include "RouterHandler.h"

namespace http {

    namespace router {

        class Router {
        public:
            using HandlerPtr = std::shared_ptr<RouterHandler>;
            using HandlerCallback = std::function<void(const HttpRequest&, HttpResponse*)>; 

            // 路由键 = HTTP方法 + 路径
            struct RouteKey {
                HttpRequest::Method method;
                std::string path;

                bool operator==(const RouteKey& other) const {
                    return method == other.method && path == other.path;
                }
            };

            // 为 RouteKey 定义哈希函数
            struct RouteKeyHash {
                std::size_t operator()(const RouteKey& key) const {
                    return std::hash<int>()(static_cast<int>(key.method)) ^ std::hash<std::string>()(key.path);
                }
            };

            // 静态路由注册
            void registerHandler(const HttpRequest::Method& method, const std::string& path, HandlerPtr handler);
            void registerCallback(const HttpRequest::Method& method, const std::string& path, HandlerCallback callback);

            // 动态路由注册（使用正则表达式）
            void addRegexHandler(const HttpRequest::Method& method, const std::string& path, HandlerPtr handler);
            void addRegexCallback(const HttpRequest::Method& method, const std::string& path, HandlerCallback callback);

            // 处理请求
            bool route(const HttpRequest& request, HttpResponse* response);

        private:
            // 路径转成正则表达式
            std::regex convertPathToRegex(const std::string& path);
            // 提取路径参数
            void extractPathParameters(const std::smatch& match, HttpRequest* request);

        private:
            struct RouteCallbackObj {
                HttpRequest::Method method_;
                std::regex pathRegex_;
                HandlerCallback callback_;

                RouteCallbackObj(HttpRequest::Method method, const std::regex& pathRegex, HandlerCallback callback)
                    : method_(method), pathRegex_(pathRegex), callback_(callback) {}
            };

            struct RouteHandlerObj {
                HttpRequest::Method method_;
                std::regex pathRegex_;
                HandlerPtr handler_;

                RouteHandlerObj(HttpRequest::Method method, const std::regex& pathRegex, HandlerPtr handler)
                    : method_(method), pathRegex_(pathRegex), handler_(handler) {}
            };

            std::unordered_map<RouteKey, HandlerPtr, RouteKeyHash> handlers_;
            std::unordered_map<RouteKey, HandlerCallback, RouteKeyHash> callbacks_;
            std::vector<RouteHandlerObj> regexHandlers_;
            std::vector<RouteCallbackObj> regexCallbacks_;

        };
    }
}
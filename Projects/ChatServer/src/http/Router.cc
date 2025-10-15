#include "router/Router.h"

namespace http {

    namespace router {
        
        void Router::registerCallback(const HttpRequest::Method& method, const std::string& path, HandlerCallback callback) {
            RouteKey key{method, path};
            callbacks_[key] = callback;
        }

        void Router::registerHandler(const HttpRequest::Method& method, const std::string& path, HandlerPtr handler) {
            RouteKey key{method, path};
            handlers_[key] = handler;
        }

        void Router::addRegexCallback(const HttpRequest::Method& method, const std::string& path, HandlerCallback callback) {
            std::regex pathRegex = convertPathToRegex(path);
            regexCallbacks_.emplace_back(method, pathRegex, callback);
        }

        void Router::addRegexHandler(const HttpRequest::Method& method, const std::string& path, HandlerPtr handler) {
            std::regex pathRegex = convertPathToRegex(path);
            regexHandlers_.emplace_back(method, pathRegex, handler);
        }

        std::regex Router::convertPathToRegex(const std::string& path) {
            std::string regexStr = "^";
            for (size_t i = 0; i < path.size(); ++i) {
                if (path[i] == '{') {
                    regexStr += "([^/]+)";
                    while (i < path.size() && path[i] != '}') ++i;
                } else {
                    if (std::ispunct(path[i])) {
                        regexStr += '\\';
                    }
                    regexStr += path[i];
                }
            }
            regexStr += "$";
            return std::regex(regexStr);
        }

        void Router::extractPathParameters(const std::smatch& match, HttpRequest* request) {
            for (size_t i = 1; i < match.size(); ++i) {
                request->addPathParameter("param" + std::to_string(i), match[i].str());
            }
        }

        bool Router::route(const HttpRequest& request, HttpResponse* response) {
            RouteKey key{request.method(), request.path()};

            // 先查找静态路由
            auto itHandler = handlers_.find(key);
            if (itHandler != handlers_.end()) {
                itHandler->second->handle(request, response);
                return true;
            }

            auto itCallback = callbacks_.find(key);
            if (itCallback != callbacks_.end()) {
                itCallback->second(request, response);
                return true;
            }

            // 再查找动态路由
            for (const auto& routeObj : regexHandlers_) {
                if (routeObj.method_ == request.method()) {
                    std::smatch match;
                    std::string path = request.path();
                    if (std::regex_match(path, match, routeObj.pathRegex_)) {
                        HttpRequest reqCopy = request; // 复制请求以添加路径参数
                        extractPathParameters(match, &reqCopy);
                        routeObj.handler_->handle(reqCopy, response);   // 这里传入的是修改后的请求对象
                        return true;
                    }
                }
            }

            for (const auto& routeObj : regexCallbacks_) {
                if (routeObj.method_ == request.method()) {
                    std::smatch match;
                    std::string path = request.path();
                    if (std::regex_match(path, match, routeObj.pathRegex_)) {
                        HttpRequest reqCopy = request; // 复制请求以添加路径参数
                        extractPathParameters(match, &reqCopy);
                        routeObj.callback_(reqCopy, response);
                        return true;
                    }
                }
            }

            return false; // 未找到匹配的路由
        }
    }
}

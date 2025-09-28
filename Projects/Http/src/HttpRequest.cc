#include "HttpRequest.h"

namespace http {

    std::string HttpRequest::getPathParameter(const std::string& key) const {
        auto it = pathParameters_.find(key);
        if (it != pathParameters_.end()) {
            return it->second;
        }
        return "";
    }

    std::string HttpRequest::getQueryParameter(const std::string& key) const {
        auto it = queryParameters_.find(key);
        if (it != queryParameters_.end()) {
            return it->second;
        }
        return "";
    }

    std::string HttpRequest::getHeader(const std::string& key) const {
        auto it = headers_.find(key);
        if (it != headers_.end()) {
            return it->second;
        }
        return "";
    }

    bool HttpRequest::setMethod(const char* start, const char* end) {
        std::string method(start, end);
        if (method == "GET") {
            method_ = kGet;
        }
        else if (method == "POST") {
            method_ = kPost;
        }
        else if (method == "HEAD") {
            method_ = kHead;
        }
        else if (method == "PUT") {
            method_ = kPut;
        }
        else if (method == "DELETE") {
            method_ = kDelete;
        }
        else if (method == "OPTIONS") {
            method_ = kOptions;
        }
        else {
            method_ = kInvalid;
            return false;
        }
        return true;
    }

    void HttpRequest::addQueryParameter(const char* start, const char* end) {
        std::string arg(start, end);
        std::string::size_type pos = 0;
        std::string::size_type pre = 0;

        // 解析类似 key1=value1&key2=value2 的参数
        while((pos = arg.find('&', pre)) != std::string::npos) {
            std::string subArg = arg.substr(pre, pos - pre);
            auto eqPos = subArg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = subArg.substr(0, eqPos);
                std::string value = subArg.substr(eqPos + 1);
                queryParameters_[key] = value;
            }
            else {
                queryParameters_[subArg] = "";
            }
            pre = pos + 1;
        }
        // 处理最后一个参数
        pos = pre;
        if (pos < arg.size()) {
            std::string subArg = arg.substr(pre);
            auto eqPos = subArg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = subArg.substr(0, eqPos);
                std::string value = subArg.substr(eqPos + 1);
                queryParameters_[key] = value;
            }
            else {
                queryParameters_[subArg] = "";
            }
        }
    }

    // 将请求头的\r\n去掉后，再传进来
    void HttpRequest::addHeader(const char* start, const char* end) {
        std::string arg(start, end);
        auto pos = arg.find(':');
        if (pos != std::string::npos) {
            std::string key = arg.substr(0, pos);
            // 跳过冒号后的空格
            std::string::size_type valueStart = pos + 1;
            while (valueStart < arg.size() && arg[valueStart] == ' ') {
                ++valueStart;
            }
            std::string value = arg.substr(valueStart);
            // 去掉值末尾的空格
            while (!value.empty() && value.back() == ' ') {
                value.pop_back();
            }
            headers_[key] = value;
        }
    }
}
    

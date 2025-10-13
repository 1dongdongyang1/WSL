#pragma once

#include "HttpRequest.h"
#include <muduo/net/Buffer.h>

namespace http {

    /**
     * @brief HTTP上下文封装
     * 用于处理HTTP请求的解析状态
     */
    class HttpContext {
    public:
        enum HttpRequestParseState {
            kExpectRequestLine,   // 期待请求行
            kExpectHeaders,       // 期待头部
            kExpectBody,          // 期待请求体
            kGotAll,              // 完成解析
        };

        HttpContext() :state_(kExpectRequestLine) {}

        bool parseRequest(muduo::net::Buffer* buf, muduo::Timestamp receiveTime);

        // get
        bool gotAll() const { return state_ == kGotAll; }
        const HttpRequest& request() const { return request_; }
        HttpRequest& request() { return request_; }

        // reset
        void reset() {
            state_ = kExpectRequestLine;
            HttpRequest dummy;
            request_.swap(dummy);
        }

    private:
        bool processRequestLine(const char* begin, const char* end);

    private:
        HttpRequestParseState   state_;     // 当前解析状态
        HttpRequest             request_;   // 解析得到的HTTP请求

    };
}
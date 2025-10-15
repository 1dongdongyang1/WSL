#pragma once

#include <map>
#include <string>

#include <muduo/net/Buffer.h>

namespace http {

    /**
     * @brief HTTP响应封装
     * HTTP响应格式
     * 状态行: 版本 状态码 状态消息\r\n   -> Version StatusCode StatusMessage
     * 响应头: 键:值\r\n               -> headers
     * \r\n
     * 响应体                    -> body
     */
    class HttpResponse {
    public:
        enum StatusCode {
            kUnknown,
            k200OK = 200,
            k204NoContent = 204,
            k301MovedPermanently = 301,
            k400BadRequest = 400,
            k401Unauthorized = 401,
            k403Forbidden = 403,
            k404NotFound = 404,
            k409Conflict = 409,
            k500InternalServerError = 500,
        };

        HttpResponse(bool close = true) :version_("Unknown"), statusCode_(kUnknown), statusMessage_("Unknown"),
            closeConnection_(close), isFile_(false) {}

        // get
        std::string version() const { return version_; }
        StatusCode statusCode() const { return statusCode_; }
        std::string statusMessage() const { return statusMessage_; }
        std::string getHeader(const std::string& key) const;
        const std::map<std::string, std::string>& headers() const { return headers_; }
        std::string body() const { return body_; }
        bool closeConnection() const { return closeConnection_; }
        bool isFile() const { return isFile_; }

        // set
        void setVersion(const std::string& version) { version_ = version; }
        void setStatusCode(StatusCode code) { statusCode_ = code; }
        void setStatusMessage(const std::string& message) { statusMessage_ = message; }
        void addHeader(const std::string& key, const std::string& value) { headers_[key] = value; }
        void setBody(const std::string& body) { body_ = body; }
        void setCloseConnection(bool on) { closeConnection_ = on; }
        void setIsFile(bool isFile) { isFile_ = isFile; }
        void setContentType(const std::string& contentType) { addHeader("Content-Type", contentType); }
        void setContentLength(uint64_t contentLength) { addHeader("Content-Length", std::to_string(contentLength)); }
        void setStatusLine(const std::string& version, StatusCode code, const std::string& statusMessage);
        void appendToBuffer(muduo::net::Buffer* output) const;

    private:
        std::string                         version_;       // HTTP版本
        StatusCode                          statusCode_;    // 状态码
        std::string                         statusMessage_; // 状态消息
        std::map<std::string, std::string>  headers_;       // 响应头
        std::string                         body_;          // 响应体
        bool                                closeConnection_; // 是否关闭连接
        bool                                isFile_;          // 响应体是否是文件
    };
}
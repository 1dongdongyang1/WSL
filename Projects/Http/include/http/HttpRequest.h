#pragma once

#include <map>
#include <string>
#include <unordered_map>

#include <muduo/base/Timestamp.h>

namespace http {

    /**
     * @brief HTTP请求封装
     * HTTP请求格式
     * 请求行：方法 URL 版本\r\n    -> Method Path Version  -> Path->pathParameters + queryParameters
     * 请求头: 键:值\r\n              -> headers
     * \r\n
     * 请求体                   -> content
     */
    class HttpRequest {
    public:
        enum Method {
            kInvalid,
            kGet,
            kPost,
            kHead,
            kPut,
            kDelete,
            kOptions,
        };

        HttpRequest() :method_(kInvalid), version_("Unknown") {}

        void swap(HttpRequest& that);

        // get
        Method method() const { return method_; }
        std::string version() const { return version_; }
        std::string path() const { return path_; }
        std::string getPathParameter(const std::string& key) const;
        std::string getQueryParameter(const std::string& key) const;
        muduo::Timestamp receiveTime() const { return receiveTime_; }
        std::string getHeader(const std::string& key) const;
        const std::map<std::string, std::string>& headers() const { return headers_; }
        std::string content() const { return content_; }
        uint64_t contentLength() const { return contentLength_; }

        // set
        bool setMethod(const char* start, const char* end);
        void setVersion(const char* start, const char* end) { version_.assign(start, end); }
        void setPath(const char* start, const char* end) { path_.assign(start, end); }
        void addPathParameter(const std::string& key, const std::string& value) { pathParameters_[key] = value; }
        void addQueryParameter(const char* start, const char* end);
        void setReceiveTime(muduo::Timestamp receiveTime) { receiveTime_ = receiveTime; }
        void addHeader(const char* start, const char* end);
        void setContent(const std::string& content) { content_ = content; }
        void setContentLength(uint64_t contentLength) { contentLength_ = contentLength; }

    private:
        Method                                          method_;
        std::string                                     version_;
        std::string                                     path_;              // 请求路径
        std::unordered_map<std::string, std::string>    pathParameters_;    // 路径参数
        std::unordered_map<std::string, std::string>    queryParameters_;   // 查询参数
        muduo::Timestamp                                receiveTime_;
        std::map<std::string, std::string>              headers_;
        std::string                                     content_;
        uint64_t                                        contentLength_{ 0 };
    };

}
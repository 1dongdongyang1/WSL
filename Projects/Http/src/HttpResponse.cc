#include "HttpResponse.h"

namespace http {

    void HttpResponse::appendToBuffer(muduo::net::Buffer* output) const {
        // Append status line
        output->append("HTTP/");
        output->append(version_);
        output->append(" ");
        output->append(std::to_string(statusCode_));
        output->append(" ");
        output->append(statusMessage_);
        output->append("\r\n");

        if (closeConnection_) {
            output->append("Connection: close\r\n");
        }
        else {
            output->append("Connection: keep-alive\r\n");
        }

        // Append headers
        for (const auto& header : headers_) {
            output->append(header.first);
            output->append(": ");
            output->append(header.second);
            output->append("\r\n");
        }

        output->append("\r\n");

        // Append body
        output->append(body_);
    }

    void HttpResponse::setStatusLine(const std::string& version,
        StatusCode code, const std::string& statusMessage) {
        version_ = version;
        statusCode_ = code;
        statusMessage_ = statusMessage;
    }

    std::string HttpResponse::getHeader(const std::string& key) const {
        auto it = headers_.find(key);
        if (it != headers_.end()) {
            return it->second;
        }
        return "";
    }

}  // namespace http

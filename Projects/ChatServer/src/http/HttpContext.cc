#include "http/HttpContext.h"
#include <muduo/base/Logging.h>

namespace http {
    
    bool HttpContext::parseRequest(muduo::net::Buffer* buf, muduo::Timestamp receiveTime) {
        bool ok = true;     // 解析每行请求格式是否正确
        bool hasMore = true;
        while (hasMore) {
            // 解析请求行
            if (state_ == kExpectRequestLine) {
                // 查找\r\n 指向\r
                const char* crlf = buf->findCRLF();
                if (crlf) {
                    ok = processRequestLine(buf->peek(), crlf);
                    if (ok) {
                        request_.setReceiveTime(receiveTime);
                        buf->retrieveUntil(crlf + 2);
                        state_ = kExpectHeaders;
                    }
                    else {
                        hasMore = false;
                        LOG_INFO << "HttpServer::parseRequest processRequestLine failed"; 
                    }
                }
            }
            else if (state_ == kExpectHeaders) {
                const char* crlf = buf->findCRLF();
                if (crlf) {
                    const char* colon = std::find(buf->peek(), crlf, ':');
                    if (colon < crlf) {
                        request_.addHeader(buf->peek(), crlf);
                        // LOG_INFO << "HttpServer::parseRequest addHeader: " << std::string(buf->peek(), crlf);
                    }
                    else if (buf->peek() == crlf) {
                        // 空行，结束Header
                        // 根据请求方法和Content-Length判断是否需要继续读取body
                        if (request_.method() == HttpRequest::kPost
                            || request_.method() == HttpRequest::kPut) {
                            std::string contentLength = request_.getHeader("Content-Length");
                            if (!contentLength.empty()) {
                                request_.setContentLength(std::stoi(contentLength));
                                if (request_.contentLength() > 0) {
                                    state_ = kExpectBody;
                                }
                                else {
                                    state_ = kGotAll;
                                    hasMore = false;
                                }
                            }
                            else {
                                // 没有Content-Length
                                ok = false;
                                hasMore = false;
                            }
                        }
                        else {
                            state_ = kGotAll;
                            hasMore = false;
                        }
                    }
                    else {
                        // Header行格式错误
                        ok = false;
                        hasMore = false;
                    }
                    buf->retrieveUntil(crlf + 2);
                }
                else {
                    hasMore = false;
                }
            }
            else if (state_ == kExpectBody) {
                // 检查缓冲区中是否有足够的数据
                if (buf->readableBytes() < request_.contentLength()) {
                    // 数据不完整，等待更多数据
                    hasMore = false;
                    return true;
                }

                // 只读取Content-Length指定长度
                std::string content(buf->peek(), buf->peek() + request_.contentLength());
                request_.setContent(content);

                buf->retrieve(request_.contentLength());
                state_ = kGotAll;
                hasMore = false;
            }
        }
        // LOG_INFO << "HttpServer::parseRequest ok=" << ok << ", state=" << state_;
        return ok;
    }

    // 解析请求行 
    bool HttpContext::processRequestLine(const char* start, const char* end) {
        bool succeed = false;
        const char* begin = start;
        const char* space = std::find(begin, end, ' ');
        // Method URL version
        if (space != end && request_.setMethod(begin, space)) {
            begin = space + 1;
            space = std::find(begin, end, ' ');
            // URL
            if (space != end) {
                const char* question = std::find(begin, space, '?');
                if (question != space) {    // 请求带查询参数
                    request_.setPath(begin, question);
                    request_.addQueryParameter(question + 1, space);
                }
                else {
                    request_.setPath(begin, space);
                }
                begin = space + 1;
                request_.setVersion(begin, end);
                succeed = true;
            }
        }
        return succeed;
    }


}
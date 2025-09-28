#include "HttpServer.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"


#include <muduo/base/Logging.h>
#include <muduo/net/Buffer.h>

namespace http {

    HttpServer::HttpServer(muduo::net::EventLoop* loop,
        const muduo::net::InetAddress& listenAddr,
        const std::string& nameArg)
        : listenAddr_(listenAddr)
        , server_(loop, listenAddr, nameArg)
        , httpCallback_(nullptr) {

    }

    void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn) {
        if(conn->connected()) {
            conn->setContext(HttpContext());
        }
    }
    
    void HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
        muduo::net::Buffer* buf,
        muduo::Timestamp receiveTime) {
        try {
            HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());
            if(!context->parseRequest(buf, receiveTime)) {
                LOG_ERROR << "HttpServer::onMessage no context";
                conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
                conn->shutdown();
            }

            if(context->gotAll()) {
                onRequest(conn, context->request());
                context->reset();
            }
        }
        catch(const std::exception& e) {
            LOG_ERROR << "HttpServer::onMessage exception: " << e.what();
            conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->shutdown();
        }
    }

    void HttpServer::onRequest(const muduo::net::TcpConnectionPtr& conn, const HttpRequest& request) {
        const std::string& connection = request.getHeader("Connection");
        bool close = connection == "close" ||
            (request.version() == "HTTP/1.0" && connection != "Keep-Alive");

        HttpResponse response(close);
        httpCallback_(request, &response);

        // 可以给response设置一个成员，判断是否请求的是文件，如果是文件设置为true，并且存在文件位置在这里send出去。
        muduo::net::Buffer buf;
        response.appendToBuffer(&buf);
        LOG_INFO << "Sending response:\n" << buf.toStringPiece().as_string();
        conn->send(&buf);
        if(response.closeConnection()) {
            conn->shutdown();
        }
    }

}
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
        server_.setConnectionCallback(
            std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(
            std::bind(&HttpServer::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    }

    void HttpServer::start() {
        LOG_INFO << "HttpServer[" << server_.name() << "] starts listening on" << server_.ipPort();
        server_.start();
        mainLoop_.loop();
    }

    void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn) {
        if (conn->connected()) {
            conn->setContext(HttpContext());
        }
    }

    void HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
        muduo::net::Buffer* buf,
        muduo::Timestamp receiveTime) {
        try {
            HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());
            if (!context->parseRequest(buf, receiveTime)) {
                LOG_ERROR << "HttpServer::onMessage no context";
                conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
                conn->shutdown();
            }

            if (context->gotAll()) {
                onRequest(conn, context->request());
                context->reset();
            }
        }
        catch (const std::exception& e) {
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
        if (response.closeConnection()) {
            conn->shutdown();
        }
    }

    void HttpServer::handleRequest(const HttpRequest& request, HttpResponse* response) {
        try {
            // 处理请求前的中间件
            HttpRequest mutableRequest = request; // 创建可变的请求副本
            middlewareChain_.processBefore(mutableRequest);

            // 路由处理
            if (!router_.route(mutableRequest, response)) {
                LOG_INFO << "path: " << mutableRequest.path() << " not found";
                response->setStatusCode(HttpResponse::StatusCode::k404NotFound);
                response->setStatusMessage("Not Found");
                response->setCloseConnection(true);
            }

            // 处理请求后的中间件
            middlewareChain_.processAfter(*response);
        }
        catch (const HttpResponse& res) {
            // 处理中间件抛出的响应(如CORS预检请求)
            *response = res; 
        }
        catch (const std::exception& e) {
            // 错误处理
            response->setStatusCode(HttpResponse::StatusCode::k500InternalServerError);
            response->setStatusMessage("Internal Server Error");
            response->setBody("An error occurred: " + std::string(e.what()));
            response->setCloseConnection(true);
        }
    }

}
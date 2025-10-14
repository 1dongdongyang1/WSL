#include "HttpServer.h"
#include "HttpContext.h"

#include <muduo/base/Logging.h>
#include <muduo/net/Buffer.h>

namespace http {

    // default callback
    void defaultHttpCallback(const HttpRequest& req, HttpResponse* resp) {
        (void)req;
        resp->setStatusCode(HttpResponse::k404NotFound);
        resp->setStatusMessage("Not Found");
        resp->setCloseConnection(true);
    }

    HttpServer::HttpServer(int port,
        const std::string& name,
        bool useSsl,
        muduo::net::TcpServer::Option option)
        : listenAddr_("127.0.0.1", port)
        , server_(&mainLoop_, listenAddr_, name, option)
        , httpCallback_(defaultHttpCallback)
        , router_()
        , sessionManager_(nullptr)
        , middlewareChain_()
        , sslContext_(nullptr)
        , useSsl_(useSsl) {
        server_.setConnectionCallback(
            std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(
            std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        // 默认添加CORS中间件
        addMiddleware(std::make_shared<middleware::CorsMiddleware>());
    }

    void HttpServer::start() {
        if (useSsl_) {
            ssl::SslConfig sslConfig;
            sslConfig.setCertFile("/home/ddy/Projects/Http/example/server.crt");
            sslConfig.setKeyFile("/home/ddy/Projects/Http/example/server.key");
            sslConfig.setSSLVersion(ssl::SSLVersion::TLSv1_2);
            setSslConfig(sslConfig);
        }

        LOG_INFO << "HttpServer[" << server_.name() << "] starts listening on" << server_.ipPort();
        server_.start();
        mainLoop_.loop();
    }

    void HttpServer::setSslConfig(const ssl::SslConfig& config) {
        sslContext_ = std::make_unique<ssl::SslContext>(config);
        if (!sslContext_->initialize()) {
            LOG_FATAL << "Failed to initialize SSL context";
            exit(EXIT_FAILURE);
        }
    }

    void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn) {
        if (conn->connected()) {
            if (useSsl_) {
                auto sslConn = std::make_unique<ssl::SslConnection>(conn, sslContext_.get());
                sslConn->setMessageCallback(
                    std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
                sslConn->setDecryptedCallback(
                    std::bind(&HttpServer::onDecryptedMessage, this,
                        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
                sslConnections_[conn] = std::move(sslConn);
                sslConnections_[conn]->startHandshake();
            }

            conn->setContext(HttpContext());
        }
        else {
            if (useSsl_) {
                sslConnections_.erase(conn);
            }
        }
    }

    void HttpServer::onDecryptedMessage(const muduo::net::TcpConnectionPtr& conn,
        muduo::net::Buffer* buf,
        muduo::Timestamp receiveTime) {
        HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());
        if (!context->parseRequest(buf, receiveTime)) {
            conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->shutdown();
            return;
        }

        if (context->gotAll()) {
            onRequest(conn, context->request());
            context->reset();
        }
    }

    void HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
        muduo::net::Buffer* buf,
        muduo::Timestamp receiveTime) {
        try {
            // if SSL is enabled, process encrypted data
            if (useSsl_) {
                LOG_INFO << "SSL is enabled, processing encrypted data";
                auto it = sslConnections_.find(conn);
                if (it != sslConnections_.end()) {
                    LOG_INFO << "Found SSL connection for TcpConnection";
                    it->second->onTcpRead(conn, buf, receiveTime);
                    return;
                }
            }
            HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());
            LOG_INFO << "Received data:\n" << buf->toStringPiece().as_string();

            if (!context->parseRequest(buf, receiveTime)) {
                LOG_ERROR << "HttpServer::onMessage no context";
                conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
                conn->shutdown();
            }

            if (context->gotAll()) {
                // LOG_INFO << "HttpServer::onMessage gotAll";
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

        // LOG_INFO << "request version: " << request.version();
        HttpResponse response(close);
        response.setVersion(request.version());
        handleRequest(request, &response);

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
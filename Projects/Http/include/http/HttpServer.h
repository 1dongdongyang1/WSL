#pragma once

#include <string>
#include <functional>

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

class HttpRequest;
class HttpResponse;

namespace http {

    class HttpServer : muduo::noncopyable {
    public:
        using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>; 

        HttpServer(muduo::net::EventLoop* loop,
            const muduo::net::InetAddress& listenAddr,
            const std::string& nameArg);
        ~HttpServer();

        void start(int port);
        void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }
        muduo::net::EventLoop* getLoop() const { return server_.getLoop(); }

    private:
        muduo::net::InetAddress                 listenAddr_;    
        muduo::net::EventLoop                   mainLoop_;
        muduo::net::TcpServer                   server_;
        HttpCallback                            httpCallback_;

        void onConnection(const muduo::net::TcpConnectionPtr& conn);
        void onMessage(const muduo::net::TcpConnectionPtr& conn,
            muduo::net::Buffer* buf,
            muduo::Timestamp receiveTime);
        void onRequest(const muduo::net::TcpConnectionPtr& conn, const HttpRequest& request);

    };
}


#pragma once

#include <string>
#include <functional>

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include "SessionManager.h"
#include "Router.h"

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

        void start();

        // get
        muduo::net::EventLoop* getLoop() const { return server_.getLoop(); }
        const muduo::net::InetAddress& getListenAddress() const { return listenAddr_; }
        std::string name() const { return server_.name(); }

        // set
        void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }
        void setHttpCallback(const HttpCallback& cb) { httpCallback_ = cb; }

        // router
        void Get(const std::string& path, const HttpCallback& cb) { router_.registerCallback(HttpRequest::kGet, path, cb); }
        void Get(const std::string& path, router::Router::HandlerPtr handler) { router_.registerHandler(HttpRequest::kGet, path, handler); }
        void Post(const std::string& path, const HttpCallback& cb) { router_.registerCallback(HttpRequest::kPost, path, cb); }
        void Post(const std::string& path, router::Router::HandlerPtr handler) { router_.registerHandler(HttpRequest::kPost, path, handler); }
        void addRoute(const HttpRequest::Method& method, const std::string& path, const HttpCallback& cb) { router_.addRegexCallback(method, path, cb); }
        void addRoute(const HttpRequest::Method& method, const std::string& path, router::Router::HandlerPtr handler) { router_.addRegexHandler(method, path, handler); }

        // session
        void setSessionManager(std::unique_ptr<session::SessionManager> sessionManager) { sessionManager_ = std::move(sessionManager); }
        session::SessionManager* getSessionManager() const { return sessionManager_.get(); }

    private:
        void onConnection(const muduo::net::TcpConnectionPtr& conn);
        void onMessage(const muduo::net::TcpConnectionPtr& conn,
            muduo::net::Buffer* buf,
            muduo::Timestamp receiveTime);
        void onRequest(const muduo::net::TcpConnectionPtr& conn, const HttpRequest& request);

    private:
        muduo::net::InetAddress                     listenAddr_;    
        muduo::net::EventLoop                       mainLoop_;
        muduo::net::TcpServer                       server_;
        HttpCallback                                httpCallback_;
        router::Router                              router_;
        std::unique_ptr<session::SessionManager>    sessionManager_;

    };
}


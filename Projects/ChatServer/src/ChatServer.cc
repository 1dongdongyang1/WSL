#include "ChatServer.h"
#include "session/MemorySessionStorage.h"

#include "handlers/ChatEntryHandler.h"
#include "handlers/ChatRegisterHandler.h"
#include "handlers/ChatLoginHandler.h"
#include "handlers/ChatMenuHandler.h"
#include "handlers/ChatLogoutHandler.h"

ChatServer::ChatServer(int port,
                       const std::string& name,
                       muduo::net::TcpServer::Option option)
    : httpServer_(port, name, false, option)
{
    init();
}

void ChatServer::setThreadNum(int numThreads) {
    httpServer_.setThreadNum(numThreads);
}

void ChatServer::start() {
    httpServer_.start();
}

void ChatServer::init() {
    http::MysqlUtil::init("127.0.0.1", "ddy", "123456", "http", 10);
    initSession();
    initRouter();
    initMiddleware();
}

void ChatServer::initSession() {
    auto sessionStorage = std::make_unique<http::session::MemorySessionStorage>();
    auto sessionManager = std::make_unique<http::session::SessionManager>(std::move(sessionStorage));
    setSessionManager(std::move(sessionManager));
}

void ChatServer::initRouter() {
    // Get
    httpServer_.Get("/",
        std::make_shared<ChatEntryHandler>(this));
    httpServer_.Get("/entry",
        std::make_shared<ChatEntryHandler>(this));
    httpServer_.Get("/menu",
        std::make_shared<ChatMenuHandler>(this));

    // Post
    httpServer_.Post("/register",
        std::make_shared<ChatRegisterHandler>(this));
    httpServer_.Post("/login",
        std::make_shared<ChatLoginHandler>(this));
    httpServer_.Post("/user/logout",
        std::make_shared<ChatLogoutHandler>(this));
}

void ChatServer::initMiddleware() {
    auto cors = std::make_shared<http::middleware::CorsMiddleware>();
    httpServer_.addMiddleware(cors);
}
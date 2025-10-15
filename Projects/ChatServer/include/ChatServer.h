#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <unordered_map>
#include <muduo/base/noncopyable.h>

#include "http/http/HttpServer.h"
#include "http/session/SessionManager.h"
#include "http/utils/MysqlUtil.h"
#include "http/utils/JsonUtil.h"

/**
 * 业务逻辑
 * 1. 用户注册
 * 2. 用户登录
 * 3. 用户登出
 * 4. 用户与AI聊天
 */

class ChatRegisterHandler;
class ChatLoginHandler;
class ChatLogoutHandler;
class ChatMenuHandler;
class ChatHandler;


/// @brief 聊天服务器
class ChatServer : muduo::noncopyable {
public:
    ChatServer(int port,
               const std::string& name = "ChatServer",
               muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort);
    void setThreadNum(int numThreads);
    void start();
    void initChatMessage();

private:
    // init
    void init();
    void initSession();
    void initRouter();
    void initMiddleware();

    // Getters && Setters
    http::session::SessionManager* getSessionManager() const {
        return httpServer_.getSessionManager();
    }
    void setSessionManager(std::unique_ptr<http::session::SessionManager> sessionManager) {
        httpServer_.setSessionManager(std::move(sessionManager));
    }

    // friend class
    friend class ChatRegisterHandler;
    friend class ChatLoginHandler;
    friend class ChatLogoutHandler;
    friend class ChatMenuHandler;
    friend class ChatHandler;

private:
    http::HttpServer    httpServer_;
    http::MysqlUtil     mysqlUtil_;

    std::unordered_map<int, bool> onlineUsers_; 
    std::mutex                    onlineUsersMutex_; 
};
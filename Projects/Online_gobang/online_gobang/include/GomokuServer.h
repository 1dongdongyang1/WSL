#pragma once

#include <string>
#include <memory>

#include "HttpServer.h"
#include "MysqlUtil.h"

/**
 *@brief GomokuServer class
 */
class GomokuServer {
public:
    GomokuServer(int port, const std::string& name = "GomokuServer",
        muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort);

    void setThreadNum(int numThreads);
    void start();

private:
    void initialize();
    void initializeRoutes();
    void initializeMiddleware();
    void initializeSession();

    void setSessionManager(std::unique_ptr<http::session::SessionManager> sessionManager);
    


private:
    enum class GameType {
        None,
        PlayerVsPlayer,
        PlayerVsAI
    };

    GameType currentGameType_ = GameType::None;
    http::HttpServer server_;
    http::MysqlUtil mysqlUtil_;

    // userId -> aiBotId
    
};

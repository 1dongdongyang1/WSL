#pragma once

#include "http/router/RouterHandler.h"
#include "http/utils/MysqlUtil.h"
#include "ChatServer.h"

/// @brief 登录页面登录业务
class ChatLoginHandler : public http::router::RouterHandler {
public:
    explicit ChatLoginHandler(ChatServer* server) :
        server_(server) {
    }

    void handle(const http::HttpRequest& request, http::HttpResponse* response) override;

private:
    int queryUser(const std::string& username, const std::string& password);
private:
    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};
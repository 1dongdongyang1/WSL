#pragma once

#include "http/router/RouterHandler.h"
#include "http/utils/MysqlUtil.h"
#include "ChatServer.h"

/// @brief 登录页面注册业务
class ChatRegisterHandler : public http::router::RouterHandler {
public:
    explicit ChatRegisterHandler(ChatServer* server):
        server_(server) {}

    void handle(const http::HttpRequest& request, http::HttpResponse* response) override;

private:
    int insertUser(const std::string& username, const std::string& password);
    bool isUserExist(const std::string& username);

private:
    ChatServer*     server_;
    http::MysqlUtil mysqlUtil_;
};
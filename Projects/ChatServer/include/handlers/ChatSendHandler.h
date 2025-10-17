#pragma once

#include "http/router/RouterHandler.h"
#include "utils/MysqlUtil.h"
#include "ChatServer.h"

/// @brief AI页面聊天业务
class ChatSendHandler : public http::router::RouterHandler {
public:
    explicit ChatSendHandler(ChatServer* server) :
        server_(server) {
    }
    void handle(const http::HttpRequest& request, http::HttpResponse* response) override;
private:
    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};
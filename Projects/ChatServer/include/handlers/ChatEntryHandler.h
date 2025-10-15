#pragma once

#include "http/router/RouterHandler.h"
#include "ChatServer.h"

/// @brief 登录页面
class ChatEntryHandler : public http::router::RouterHandler {
public:
    explicit ChatEntryHandler(ChatServer* server) :
        server_(server) {
    }

    void handle(const http::HttpRequest& request, http::HttpResponse* response) override;
private:
    ChatServer* server_;
};
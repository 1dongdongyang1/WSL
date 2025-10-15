#pragma once

#include "http/router/RouterHandler.h"
#include "ChatServer.h"

/// @brief 菜单页面登出业务
class ChatLogoutHandler : public http::router::RouterHandler {
public:
    explicit ChatLogoutHandler(ChatServer* server) : server_(server) {}
    void handle(const http::HttpRequest& request, http::HttpResponse* response) override;
private:
    ChatServer* server_;
};
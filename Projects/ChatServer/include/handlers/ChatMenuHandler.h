#pragma once

#include "http/router/RouterHandler.h"
#include "ChatServer.h"

/// @brief 菜单页面
class ChatMenuHandler : public http::router::RouterHandler {
public:
    explicit ChatMenuHandler(ChatServer* server) :
        server_(server) {
    }

    void handle(const http::HttpRequest& request, http::HttpResponse* response) override;
private:
    ChatServer* server_;
};
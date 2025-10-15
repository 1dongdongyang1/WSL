#pragma once

#include "router/RouterHandler.h"
#include "ChatServer.h"

/// @brief AI聊天页面
class ChatHandler : public http::router::RouterHandler {
public:
    explicit ChatHandler(ChatServer* server) :
        server_(server) {
    }
    void handle(const http::HttpRequest& request, http::HttpResponse* response) override;
private:
    ChatServer* server_;
};
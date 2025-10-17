#pragma once

#include "http/router/RouterHandler.h"
#include "ChatServer.h"

/// @brief AI页面获取所有会话id业务
class ChatSessionHandler : public http::router::RouterHandler {
public:
    explicit ChatSessionHandler(ChatServer* server) :
        server_(server) {
    }

    void handle(const http::HttpRequest& request, http::HttpResponse* response) override;
private:
    ChatServer* server_;
};
#pragma once

#include "router/RouterHandler.h"
#include "ChatServer.h"

/// @brief AI聊天页面同步历史数据业务
class ChatHistoryHandler : public http::router::RouterHandler {
public:
    explicit ChatHistoryHandler(ChatServer* server) :
        server_(server) {
    }
    void handle(const http::HttpRequest& request, http::HttpResponse* response) override;
private:
    ChatServer* server_;
};
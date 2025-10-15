#include "handlers/ChatLogoutHandler.h"

void ChatLogoutHandler::handle(const http::HttpRequest& request, http::HttpResponse* response) {
    auto contentType = request.getHeader("Content-Type");
    if (contentType != "application/json" || contentType.empty() || request.content().empty()) {
        response->setStatusLine(request.version(), http::HttpResponse::k400BadRequest, "Bad Request");
        response->setCloseConnection(true);
        response->setContentLength(0);
        response->setContentType("application/json");
        response->setBody("");
        return;
    }

    try {
        auto session = server_->getSessionManager()->getSession(request, response);
        int userId = std::stoi(session->get("userId"));
        session->clear();
        server_->getSessionManager()->destroySession(session->sessionId());

        {
            std::lock_guard<std::mutex> lock(server_->onlineUsersMutex_);
            server_->onlineUsers_.erase(userId);
        }        

        json successResp;
        successResp["status"] = "success";
        successResp["message"] = "Logout successful";
        std::string respBody = successResp.dump();

        response->setStatusLine(request.version(), http::HttpResponse::k200OK, "OK");
        response->setContentType("application/json");
        response->setContentLength(respBody.size());
        response->setBody(respBody);
        response->setCloseConnection(true);
        return;
    }
    catch (const std::exception& e) {
        json errorResp;
        errorResp["status"] = "error";
        errorResp["message"] = e.what();
        std::string respBody = errorResp.dump();

        response->setStatusLine(request.version(), http::HttpResponse::k500InternalServerError, "Internal Server Error");
        response->setContentType("application/json");
        response->setContentLength(respBody.size());
        response->setBody(respBody);
        response->setCloseConnection(true);
        return;
    }

}
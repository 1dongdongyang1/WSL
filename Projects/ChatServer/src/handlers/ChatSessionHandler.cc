#include "handlers/ChatSessionHandler.h"

void ChatSessionHandler::handle(const http::HttpRequest& request, http::HttpResponse* response) {
    try {
        auto session = server_->getSessionManager()->getSession(request, response);
        if (session->get("isLoggedIn") != "true") {
            json errorResp;
            errorResp["status"] = "error";
            errorResp["message"] = "Unauthorized. Please log in.";
            std::string respBody = errorResp.dump();

            response->setStatusLine(request.version(), http::HttpResponse::k401Unauthorized, "Unauthorized");
            response->setContentType("application/json");
            response->setContentLength(respBody.size());
            response->setBody(respBody);
            response->setCloseConnection(true);
            return;
        }

        int userId = std::stoi(session->get("userId"));

        json successResp;
        std::vector<std::string> sessionIds;
        {
            std::lock_guard<std::mutex> lock(server_->chatInformationMutex_);
            auto& userSessions = server_->chatInformation_[userId];
            for (const auto& pair : userSessions) {
                sessionIds.push_back(pair.first);
            }
        }
        successResp["status"] = "success";
        for (const auto& id : sessionIds) {
            json s;
            s["sessionId"] = id;
            s["name"] = "会话" + id;
            successResp["sessions"].push_back(s);
        }
        std::string respBody = successResp.dump();

        response->setStatusLine(request.version(), http::HttpResponse::k200OK, "OK");
        response->setContentType("application/json");
        response->setContentLength(respBody.size());
        response->setBody(respBody);
        response->setCloseConnection(false);
    } catch (const std::exception& e) {
        LOG_ERROR << "Error in ChatSessionHandler: " << e.what();
        response->setStatusLine(request.version(), http::HttpResponse::k500InternalServerError, "Internal Server Error");
        response->setCloseConnection(true);
        response->setContentLength(0);
        response->setContentType("application/json");
        response->setBody("");
    }
}

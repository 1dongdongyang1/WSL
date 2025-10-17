#include "handlers/ChatHistoryHandler.h"

void ChatHistoryHandler::handle(const http::HttpRequest& request, http::HttpResponse* response) {
    try {
        auto session = server_->getSessionManager()->getSession(request, response);
        if(session->get("isLoggedIn") != "true") {
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
        std::string username = session->get("username");

        std::string sessionId;
        auto body = request.content();
        if(!body.empty()) {
            json requestBody = json::parse(body);
            sessionId = requestBody["sessionId"];
        }

        std::vector<std::pair<std::string, long long>> chatHistory;
        std::shared_ptr<AIHelper> AIHelperPtr;
        {
            std::lock_guard<std::mutex> lock(server_->chatInformationMutex_);
            auto& userSessions = server_->chatInformation_[userId];
            if(userSessions.find(sessionId) == userSessions.end()) {
                userSessions[sessionId] = std::make_shared<AIHelper>();
            }
            AIHelperPtr = userSessions[sessionId];
            chatHistory = AIHelperPtr->getMessages();
        }

        json successResp;
        successResp["status"] = "success";
        successResp["history"] = json::array();

        for(size_t i = 0; i < chatHistory.size(); ++i) {
            json messageJson;
            messageJson["is_user"] = (i % 2 == 0);
            messageJson["content"] = chatHistory[i].first;
            successResp["history"].push_back(messageJson);
        }

        std::string respBody = successResp.dump();

        response->setStatusLine(request.version(), http::HttpResponse::k200OK, "OK");
        response->setContentType("text/html");
        response->setContentLength(respBody.size());
        response->setBody(respBody);
        response->setCloseConnection(false);
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
    }
}
#include "handlers/ChatSendHandler.h"
#include "utils/AISessionIdGenerator.h"
#include "utils/LogUtil.h"

void ChatSendHandler::handle(const http::HttpRequest& request, http::HttpResponse* response) {
    try {
        APP_LOG_INFO("ChatSendHandler called");

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
        std::string username = session->get("username");

        std::string userQuestion;
        std::string modelType;
        std::string sessionId;

        auto body = request.content();
        if(!body.empty()) {
            auto reqJson = json::parse(body);
            userQuestion = reqJson["question"];
            sessionId = reqJson.value("sessionId", "");
            modelType = reqJson.value("modelType", "1");
        }

        // 如果没有提供 sessionId 或 sessionId 为空，创建新会话
        bool isNewSession = false;
        if (sessionId.empty() || sessionId == "temp" || sessionId == "default") {
            sessionId = AISessionIdGenerator().generate();
            isNewSession = true;
        }

        std::shared_ptr<AIHelper> AIHelperPtr;
        {
            std::lock_guard<std::mutex> lock(server_->chatInformationMutex_);
            auto& userSessions = server_->chatInformation_[userId]; 
            if(userSessions.find(sessionId) == userSessions.end()) {
                userSessions[sessionId] = std::make_shared<AIHelper>();
            }
            AIHelperPtr = userSessions[sessionId];
        }

        std::string aiInformation = AIHelperPtr->chat(userId, username, sessionId, userQuestion, modelType);
        
        json successResp;
        successResp["status"] = "success";
        successResp["sessionId"] = sessionId;
        successResp["Information"] = aiInformation;
        std::string respBody = successResp.dump();

        response->setStatusLine(request.version(), http::HttpResponse::k200OK, "OK");
        response->setContentType("application/json");
        response->setContentLength(respBody.size());
        response->setBody(respBody);
        response->setCloseConnection(false);
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

#include "handlers/ChatCreateAndSendHandler.h"
#include "utils/AISessionIdGenerator.h"
#include "utils/LogUtil.h"

void ChatCreateAndSendHandler::handle(const http::HttpRequest& request, http::HttpResponse* response) {
    try {
        APP_LOG_INFO("ChatCreateAndSendHandler called");

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

        auto body = request.content();
        if (!body.empty()) {
            auto reqJson = json::parse(body);
            userQuestion = reqJson["question"];
            modelType = reqJson.value("modelType", "1");
        }

        std::string sessionId = AISessionIdGenerator().generate();

        std::shared_ptr<AIHelper> AIHelperPtr;
        {
            std::lock_guard<std::mutex> lock(server_->chatInformationMutex_);
            auto& userSessions = server_->chatInformation_[userId];
            if (userSessions.find(sessionId) == userSessions.end()) {
                userSessions[sessionId] = std::make_shared<AIHelper>();
            }
            AIHelperPtr = userSessions[sessionId];
        }

        json successResp;
        std::string aiInformation = AIHelperPtr->chat(userId, username, sessionId, userQuestion, modelType);
        successResp["status"] = "success";
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

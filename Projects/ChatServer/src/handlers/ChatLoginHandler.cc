#include "handlers/ChatLoginHandler.h"

void ChatLoginHandler::handle(const http::HttpRequest& request, http::HttpResponse* response) {
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
        json reqJson = json::parse(request.content());
        std::string username = reqJson["username"];
        std::string password = reqJson["password"];

        int userId = queryUser(username, password);
        if (userId != -1) {
            auto session = server_->getSessionManager()->getSession(request, response);

            session->set("userId", std::to_string(userId));
            session->set("username", username);
            session->set("isLoggedIn", "true");

            if (server_->onlineUsers_.find(userId) == server_->onlineUsers_.end() || !server_->onlineUsers_[userId]) {
                {
                    std::lock_guard<std::mutex> lock(server_->onlineUsersMutex_);
                    server_->onlineUsers_[userId] = true;
                }
                json successResp;
                successResp["status"] = "success";
                successResp["message"] = "Login successful";
                successResp["userId"] = userId;
                std::string respBody = successResp.dump();

                response->setStatusLine(request.version(), http::HttpResponse::k200OK, "OK");
                response->setContentType("application/json");
                response->setContentLength(respBody.size());
                response->setBody(respBody);
                response->setCloseConnection(false);
                return;
            }
            else {
                json errorResp;
                errorResp["status"] = "error";
                errorResp["message"] = "User already logged in";
                std::string respBody = errorResp.dump();

                response->setStatusLine(request.version(), http::HttpResponse::k403Forbidden, "Forbidden");
                response->setContentType("application/json");
                response->setContentLength(respBody.size());
                response->setBody(respBody);
                response->setCloseConnection(false);
                return;
            }
        }
        else {
            json errorResp;
            errorResp["status"] = "error";
            errorResp["message"] = "Invalid username or password";
            std::string respBody = errorResp.dump();

            response->setStatusLine(request.version(), http::HttpResponse::k401Unauthorized, "Unauthorized");
            response->setContentType("application/json");
            response->setContentLength(respBody.size());
            response->setBody(respBody);
            response->setCloseConnection(false);
            return;
        }
    }
    catch (const std::exception& e) {
        json errorResp;
        errorResp["status"] = "error";
        errorResp["message"] = e.what();
        std::string respBody = errorResp.dump();

        response->setStatusLine(request.version(), http::HttpResponse::k400BadRequest, "Bad Request");
        response->setContentType("application/json");
        response->setContentLength(respBody.size());
        response->setBody(respBody);
        response->setCloseConnection(false);
        return;
    }
}

int ChatLoginHandler::queryUser(const std::string& username, const std::string& password) {
    auto res = mysqlUtil_.executeQuery("SELECT id FROM users WHERE username = ? AND password = ?", username, password);
    if (res->next()) {
        return res->getInt("id");
    }
    return -1;
}

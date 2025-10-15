#include "handlers/ChatRegisterHandler.h"

void ChatRegisterHandler::handle(const http::HttpRequest& request, http::HttpResponse* response) {
    json parsed = json::parse(request.content());
    std::string username = parsed["username"];
    std::string password = parsed["password"];

    int userId = insertUser(username, password);
    if (userId != -1) {
        json successResp;
        successResp["status"] = "success";
        successResp["userId"] = userId;
        successResp["message"] = "Registration successful";
        std::string respBody = successResp.dump();

        response->setStatusLine(request.version(), http::HttpResponse::k200OK, "OK");
        response->setContentType("application/json");
        response->setContentLength(respBody.size());
        response->setBody(respBody);
        response->setCloseConnection(false);
    }
    else {
        json errorResp;
        errorResp["status"] = "error";
        errorResp["message"] = "User already exists";
        std::string respBody = errorResp.dump();

        response->setStatusLine(request.version(), http::HttpResponse::k409Conflict, "Conflict");
        response->setContentType("application/json");
        response->setContentLength(respBody.size());
        response->setBody(respBody);
        response->setCloseConnection(false);
    }
}

int ChatRegisterHandler::insertUser(const std::string& username, const std::string& password) {
    if (!isUserExist(username)) {
        auto res = mysqlUtil_.executeUpdate("INSERT INTO users (username, password) VALUES (?, ?)", username, password);
        if (res > 0) {
            auto queryRes = mysqlUtil_.executeQuery("SELECT id FROM users WHERE username = ?", username);
            if (queryRes->next()) {
                return queryRes->getInt("id");
            }
        }
    }
    return -1;
}

bool ChatRegisterHandler::isUserExist(const std::string& username) {
    auto res = mysqlUtil_.executeQuery("SELECT id FROM users WHERE username = ?", username);
    return res->next();
}
    
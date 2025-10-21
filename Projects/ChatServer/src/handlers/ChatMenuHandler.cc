#include "handlers/ChatMenuHandler.h"
#include "http/utils/FileUtil.h"

void ChatMenuHandler::handle(const http::HttpRequest& request, http::HttpResponse* response) {
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

        std::string reqFile("/home/ddy/Projects/ChatServer//resource/menu.html");
        FileUtil fileOperator(reqFile);
        if (!fileOperator.isValid()) {
            LOG_WARN << "File not found: " << reqFile;
            fileOperator.resetDefaultFile("/home/ddy/Projects/ChatServer/resource/404.html");
        }

        std::vector<char> buffer;
        fileOperator.readFile(buffer);
        std::string body(buffer.begin(), buffer.end());

        size_t headEnd = body.find("</head>");
        if (headEnd != std::string::npos) {
            std::string userInfo = "<script>const userId = '" + std::to_string(userId) + "';</script>\n";
            body.insert(headEnd, userInfo);
        }

        response->setStatusLine(request.version(), http::HttpResponse::k200OK, "OK");
        response->setContentType("text/html");
        response->setContentLength(body.size());
        response->setBody(body);
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
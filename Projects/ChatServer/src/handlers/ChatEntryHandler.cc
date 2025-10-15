#include "handlers/ChatEntryHandler.h"
#include "http/utils/FileUtil.h"

void ChatEntryHandler::handle(const http::HttpRequest& request, http::HttpResponse* response) {
    std::string reqFile;
    reqFile.append("/home/ddy/Projects/ChatServer/resource/entry.html");
    FileUtil fileOperator(reqFile);
    if (!fileOperator.isValid()) {
        LOG_WARN << "File not found: " << reqFile;
        fileOperator.resetDefaultFile("/home/ddy/Projects/ChatServer/resource/404.html");
    }

    std::vector<char> buffer;
    fileOperator.readFile(buffer);
    std::string body(buffer.begin(), buffer.end());

    response->setStatusLine(request.version(), http::HttpResponse::k200OK, "OK");
    response->setContentType("text/html");
    response->setContentLength(body.size());
    response->setBody(body);
    response->setCloseConnection(false);
}
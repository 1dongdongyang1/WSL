#pragma once

#include "RouterHandler.h"
#include "GomokuServer.h"
#include "FileUtil.h"

class EntryHandler : public http::router::RouterHandler {
public:
    EntryHandler(GomokuServer* server) : server_(server) {}
    void handle(const http::HttpRequest& request, http::HttpResponse* response) override;
private:
    GomokuServer* server_;
};

void EntryHandler::handle(const http::HttpRequest& request, http::HttpResponse* response) {
    std::string reqFile;
    reqFile.append("../../resources/entry.html");
    FileUtil fileUtil(reqFile);
    if (!fileUtil.isValid()) {
        response->setStatusCode(http::HttpResponse::k404NotFound);
        response->setStatusMessage("Not Found");
        response->setCloseConnection(true);
        fileUtil.resetDefaultFile("../../resources/NotFound.html");
        return;
    }

    // Read the file content
    std::vector<char> fileContent;
    fileUtil.readFile(fileContent);

    // Set the response body
    response->setStatusLine(request.version(), http::HttpResponse::k200OK, "OK");
    response->setCloseConnection(false);   
    response->setContentType("text/html");
    response->setContentLength(fileContent.size());
    response->setBody(std::string(fileContent.begin(), fileContent.end()));
}
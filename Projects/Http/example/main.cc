#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#include <iostream>
#include <thread>

using namespace std;
using namespace http;

int main() {
    HttpServer server(8080);
    server.Get("/hello", [](const HttpRequest& req, HttpResponse* resp) {
        resp->setStatusCode(HttpResponse::k200OK);
        resp->setStatusMessage("OK");
        resp->setContentType("text/plain");
        resp->setBody("Hello, World!\n");
        resp->setCloseConnection(false);
    });

    server.setThreadNum(4); // 设置线程数
    server.start();


    

    return 0;
}


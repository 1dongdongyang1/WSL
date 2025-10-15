#include "http/HttpServer.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "utils/MysqlUtil.h"
#include "session/DBSessionStorage.h"
#include "utils/db/DbConnectionPool.h"

#include <iostream>
#include <thread>

using namespace std;
using namespace http;

/**
 *  待测试功能：
 *  1. 会话管理         ok
 *      a. 会话管理是框架的核心功能之一，确保会话的创建、维护和销毁工作正常。
 *      b. 业务逻辑中正确使用会话数据。
 *  2. SSL支持         ok 不稳定，服务器会异常退出
 *  3. 数据库连接池     ok
 *      a. 确保数据库连接池的初始化和连接管理工作正常。
 *      b. 使用数据库是业务层的事情，框架只提供连接池功能。
 */
int main() {

    // session::SessionManager sessionManager(std::make_unique<session::MemorySessionStorage>());
    MysqlUtil::init("127.0.0.1", "ddy", "123456", "http", 10);
    auto sessionManager = std::make_unique<session::DBSessionStorage>(3600);

    HttpServer server(8080, "TestHttpServer", false);
    server.setSessionManager(std::make_unique<session::SessionManager>(std::move(sessionManager)));

    server.Get("/hello", [](const HttpRequest& req, HttpResponse* resp) {
        resp->setStatusCode(HttpResponse::k200OK);
        resp->setStatusMessage("OK");
        resp->setContentType("text/plain");
        resp->setBody("Hello, World!\n");
        resp->setCloseConnection(true);
        });
    server.Get("/favicon.ico", [](const HttpRequest& req, HttpResponse* resp) {
        resp->setStatusCode(HttpResponse::k200OK);
        resp->setStatusMessage("OK");
        resp->setContentType("image/x-icon");
        resp->setBody(""); // 空响应即可
        resp->setCloseConnection(true);
        });
    server.Get("/visit", [&](const HttpRequest& req, HttpResponse* resp) {
        auto session = server.getSessionManager()->getSession(req, resp);
        int visitCount = 1;
        if (session) {
            std::string countStr = session->get("visitCount");
            if (!countStr.empty()) {
                visitCount = std::stoi(countStr) + 1;
            }
            session->set("visitCount", std::to_string(visitCount));
        }

        resp->setStatusCode(HttpResponse::k200OK);
        resp->setStatusMessage("OK");
        resp->setContentType("text/plain");
        resp->setBody("Visit count: " + std::to_string(visitCount) + "\n");
        resp->setCloseConnection(true);
        });

    server.setThreadNum(4); // 设置线程数
    server.start();




    return 0;
}


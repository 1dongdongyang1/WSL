#include "GomokuServer.h"
#include <muduo/base/Logging.h>

GomokuServer::GomokuServer(int port, const std::string& name, muduo::net::TcpServer::Option option)
    : server_(port, name, false, option)
    , mysqlUtil_() {
    initialize();
}

void GomokuServer::setThreadNum(int numThreads) {
    server_.setThreadNum(numThreads);
}

void GomokuServer::start() {
    server_.start();
}

void GomokuServer::initialize() {
    http::MysqlUtil::init("127.0.0.1", "ddy", "123456", "gobang");
    initializeRoutes();
    initializeMiddleware();
    initializeSession();
}

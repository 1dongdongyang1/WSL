#include "chatserver.hpp"
#include "chatservice.hpp"

#include <iostream>
#include <signal.h>
using namespace std;

// 处理服务器 ctrl+c 结束后, 重置user的状态信息
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char** argv)
{
    signal(SIGINT, resetHandler);

    // 解析通过命令行传递的ip和port
    const char* ip = "127.0.0.1";
    uint16_t port = 8081;

    if (argc == 3)
    {
        ip = argv[1];
        port = atoi(argv[2]);
    }

    EventLoop loop;
    InetAddress addr(port, ip);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}
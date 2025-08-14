#pragma once

#include "log.hpp"
#include "protocol.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <functional>

namespace server
{
    using namespace std;
    enum {
        USAGE_ERR = 1,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR,
    };

    static const uint16_t gport = 8080;
    static const int gbacklog = 5;

    typedef function<bool(const Request& req, Response& resp)> func_t;

    // 保证解耦
    void handlerEnter(int sock, func_t func)
    {
        string inbuffer;
        while (true)
        {
            // 1.读取
            // 1.1如何保证读到的消息是一个完整的请求
            std::string req_text, req_str;
            if (!recvPackage(sock, inbuffer, &req_text)) return;
            // 1.2我们保证req_text里一定是一个完整的请求
            if (!deLength(req_text, &req_str)) return;
            // 2.对请求Resquest,进行反序列化
            // 2.1得到结构化的请求
            Request req;
            if (!req.deserialize(req_str)) return;

            // 3.计算处理---业务逻辑
            // 3.1得到结构化的响应
            Response resp;
            func(req, resp);    // req的处理结果, 全部放入到了resp

            // 4.对响应Response,进行序列化
            // 4.1 得到了一个"字符串"
            std::string resp_str;
            resp.serialize(&resp_str);

            // 5.发送响应
            // 5.1构建成为一个完整的报文
            std::string send_string = enLength(resp_str);
            send(sock, send_string.c_str(), send_string.size(), 0); //发送有问题
        }
    }


    class CalServer
    {
    public:
        CalServer(const uint16_t& port = gport)
            :_listensock(-1), _port(port)
        {
        }
        void initServer()
        {
            // 1.创建套接字
            _listensock = socket(AF_INET, SOCK_STREAM, 0);
            if (_listensock == -1)
            {
                logMessage(FATAL, "create socket error");
                exit(SOCKET_ERR);
            }
            logMessage(NORMAL, "create socket success: %d", _listensock);

            // 2.bind绑定自己的网络信息
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if (bind(_listensock, (struct sockaddr*)&local, sizeof(local)) == -1)
            {
                logMessage(FATAL, "bind socket error");
                exit(BIND_ERR);
            }
            logMessage(NORMAL, "bind socket success");

            // 3.监听
            if (listen(_listensock, gbacklog) == -1)   //TODO
            {
                logMessage(FATAL, "listen socket error");
                exit(LISTEN_ERR);
            }
            logMessage(NORMAL, "listen socket success");
        }
        void start(func_t func)
        {
            for (;;)
            {
                // 4.server获取新连接
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                // sock,和client进行通信的socket
                int sock = accept(_listensock, (struct sockaddr*)&peer, &len);
                if (sock == -1)
                {
                    logMessage(ERROR, "accept error, next");
                    continue;
                }
                logMessage(NORMAL, "accept a new link success, get new sock: %d", sock);
                cout << "sock: " << sock << endl;

                // version 2 多进程(1)
                pid_t id = fork();
                if (id == 0)
                {
                    // child
                    close(_listensock);
                    if (fork() > 0) exit(0);

                    // grandchild
                    handlerEnter(sock, func);
                    exit(0);
                }
                close(sock);
                // parent
                pid_t ret = waitpid(id, nullptr, 0);
                if (ret > 0) logMessage(NORMAL, "wait success: %d", ret);
            }
        }
        ~CalServer() {}
    private:
        int _listensock;  //不是用来通信的，用于监听链接到来
        uint16_t _port;
    };
}
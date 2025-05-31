#pragma once

#include "Protocol.hpp"
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

    using func_t = function<void(const HttpRequest&, HttpResponse&)>;

    class HttpServer
    {
    public:
        HttpServer(func_t func, const uint16_t& port = gport)
            :_func(func), _listensock(-1), _port(port)
        {
        }
        void initServer()
        {
            // 1.创建套接字
            _listensock = socket(AF_INET, SOCK_STREAM, 0);
            if (_listensock == -1)
            {
                exit(SOCKET_ERR);
            }

            // 2.bind绑定自己的网络信息
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if (bind(_listensock, (struct sockaddr*)&local, sizeof(local)) == -1)
            {
                exit(BIND_ERR);
            }

            // 3.监听
            if (listen(_listensock, gbacklog) == -1)   //TODO
            {
                exit(LISTEN_ERR);
            }
        }
        void HandlerHttp(int sock)
        {
            // demo
            char buffer[4096];
            HttpRequest req;
            HttpResponse resp;
            size_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] = 0;
                req.inbuffer = buffer;
                req.parse();
                _func(req, resp);
                send(sock, resp.outbuffer.c_str(), resp.outbuffer.size(), 0);
            }
        }
        void start()
        {
            for (;;)
            {
                // 4.server获取新连接
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                int sock = accept(_listensock, (struct sockaddr*)&peer, &len);
                if (sock == -1)
                {
                    continue;
                }

                pid_t id = fork();
                if (id == 0)
                {
                    close(_listensock);
                    if (fork() > 0) exit(0);
                    HandlerHttp(sock);
                    exit(0);
                }
                close(sock);
                // parent
                waitpid(id, nullptr, 0);
            }
        }
        ~HttpServer() {}
    private:
        int _listensock;  //不是用来通信的，用于监听链接到来
        uint16_t _port;
        func_t _func;
    };
}
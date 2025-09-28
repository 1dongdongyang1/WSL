#pragma once

#include "Log.hpp"
#include "Err.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

const static int backlog = 32;
const static int defaultsock = -1;

class Sock
{
public:
    Sock():_listensock(defaultsock) {}
    ~Sock()
    {
        Close();
    }
    void Close()
    {
        if (_listensock != defaultsock) close(_listensock);
    }
public:
    void Socket()
    {
        // 1.创建套接字
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensock == -1)
        {
            logMessage(FATAL, "create socket error");
            exit(SOCKET_ERR);
        }
        logMessage(NORMAL, "create socket success: %d", _listensock);
        int opt = 1;
        setsockopt(_listensock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    }

    void Bind(int port)
    {
        // 2.bind绑定自己的网络信息
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        if (bind(_listensock, (struct sockaddr*)&local, sizeof(local)) == -1)
        {
            logMessage(FATAL, "bind socket error");
            exit(BIND_ERR);
        }
        logMessage(NORMAL, "bind socket success");
    }

    void Listen()
    {
        // 3.监听
        if (listen(_listensock, backlog) == -1)
        {
            logMessage(FATAL, "listen socket error");
            exit(LISTEN_ERR);
        }
        logMessage(NORMAL, "listen socket success");
    }

    int Accept(std::string* clientip, uint16_t* clientport, int* err)
    {
        // 4.server获取新连接
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        // sock,和client进行通信的socket
        int sock = accept(_listensock, (struct sockaddr*)&peer, &len);
        *err = errno;
        if (sock == -1) logMessage(ERROR, "accept error, next");
        else
        {
            logMessage(NORMAL, "accept a new link success, get new sock: %d", sock);
            *clientip = inet_ntoa(peer.sin_addr);
            *clientport = ntohs(peer.sin_port);
        }
        return sock;
    }
    int Fd()
    {
        return _listensock;
    }
private:
    int _listensock;
};

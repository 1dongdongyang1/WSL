#pragma once

#include "sock.hpp"
#include <iostream>
#include <functional>
#include <string>

namespace select_ns
{
    static const int defaultport = 8080;
    static const int fdnum = sizeof(fd_set) * 8;
    static const int defaultfd = -1;

    using func_t = std::function<std::string(const std::string&)>;

    class SelectServer
    {
    public:
        SelectServer(func_t func, int port = defaultport) :_func(func), _port(port), _listensock(-1), _fdarray(nullptr) {}
        ~SelectServer()
        {
            if (_listensock > 0) close(_listensock);
            if (_fdarray) delete[]_fdarray;
        }
        void initServer()
        {
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);
            _fdarray = new int[fdnum];
            for (int i = 0; i < fdnum; i++) _fdarray[i] = defaultfd;
            _fdarray[0] = _listensock;  //不变了
        }
        void start()
        {
            for (;;)
            {
                fd_set rfds;
                FD_ZERO(&rfds);
                int maxfd = _fdarray[0];
                for (int i = 0; i < fdnum; i++)
                {
                    if (_fdarray[i] == defaultfd) continue;
                    FD_SET(_fdarray[i], &rfds);     //合法fd全部添加到读文件描述符集里
                    if (maxfd < _fdarray[i]) maxfd = _fdarray[i];   //更新所有fd中最大的fd
                }
                logMessage(NORMAL, "max fd is %d", maxfd);

                // struct timeval timeout = { 1,0 };
                // int n = select(_listensock + 1, &rdfs, nullptr, nullptr, &timeout);
                int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
                if (n == 0) logMessage(NORMAL, "timeout...");
                else if (n == -1) logMessage(WARNING, "select error, code: %d, err string: %s", errno, strerror(errno));
                else
                {
                    logMessage(NORMAL, "have event ready");
                    HandlerReadEvent(rfds);
                }

                // std::string clientip;
                // uint16_t clientport;
                // int sock = Sock::Accept(_listensock, &clientip, &clientport); // accept = 等 + 获取
                // if (sock < 0) continue;
            }
        }
        void HandlerReadEvent(fd_set& rdfs)
        {
            for (int i = 0; i < fdnum; i++)
            {
                if (_fdarray[i] == defaultfd) continue;
                if (FD_ISSET(_fdarray[i], &rdfs) && _fdarray[i] == _listensock) Accepter(_fdarray[i]);
                else if(FD_ISSET(_fdarray[i], &rdfs)) Recver(_fdarray[i], i);

            }
        }
        void Print()
        {
            std::cout << "fd list ";
            for (int i = 0; i < fdnum; i++)
            {
                if (_fdarray[i] == defaultfd) continue;
                std::cout << _fdarray[i] << " ";
            }
            std::cout << std::endl;
        }
        void Accepter(int listensock)
        {
                // select 告诉我, listensock读事件就绪
                std::string clientip;
                uint16_t clientport;
                int sock = Sock::Accept(listensock, &clientip, &clientport); // accept = 等 + 获取
                if (sock < 0) return;
                logMessage(NORMAL, "accept success [%s:%d]", clientip.c_str(), clientport);
                //将新的sock托管给select,即将sock添加到_fdarray的数组中
                int i = 0;
                for (; i < fdnum; i++)
                {
                    if (_fdarray[i] != defaultfd) continue;
                    else break;
                }
                if (i == fdnum)
                {
                    logMessage(WARNING, "server is full, please wait");
                    close(sock);
                }
                _fdarray[i] = sock;
                Print();
        }
        void Recver(int sock, int pos)
        {
            // 1.读取request
            char buffer[1024];
            ssize_t s = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (s > 0)
            {
                buffer[s-2] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if (s == 0)
            {
                close(sock);
                _fdarray[pos] = defaultfd;
                logMessage(NORMAL, "client quit");
                return;
            }
            else
            {
                close(sock);
                _fdarray[pos] = defaultfd;
                logMessage(ERROR, "client quit: %s", strerror(errno));
                return;
            }

            // 2.处理request
            std::string response = _func(buffer);

            // 3.发送response  bug
            write(sock, response.c_str(), response.size());
        }
    private:
        int _port;
        int _listensock;
        int* _fdarray;
        func_t _func;
    };
}
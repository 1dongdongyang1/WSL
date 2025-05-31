#pragma once

#include "sock.hpp"
#include <iostream>
#include <functional>
#include <string>
#include <poll.h>

namespace poll_ns
{
    static const int defaultport = 8080;
    static const int num = 1024;
    static const int defaultfd = -1;

    using func_t = std::function<std::string(const std::string&)>;

    class PollServer
    {
    public:
        PollServer(func_t func, int port = defaultport) :_func(func), _port(port), _listensock(-1), _rfds(nullptr) {}
        ~PollServer()
        {
            if (_listensock > 0) close(_listensock);
            if (_rfds) delete[]_rfds;
        }
        void initServer()
        {
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);
            _rfds = new struct pollfd[num];
            for (int i = 0; i < num; i++) ResetItem(i);
            _rfds[0].fd = _listensock;  
            _rfds[0].events = POLLIN;
        }
        void start()
        {
            int timeout = -1;
            for (;;)
            {
                int n = poll(_rfds, num, timeout);
                if (n == 0) logMessage(NORMAL, "timeout...");
                else if (n == -1) logMessage(WARNING, "poll error, code: %d, err string: %s", errno, strerror(errno));
                else
                {
                    logMessage(NORMAL, "have event ready");
                    HandlerReadEvent();
                }
                sleep(1);
            }
        }
        void HandlerReadEvent()
        {
            for (int i = 0; i < num; i++)
            {
                if (_rfds[i].fd == defaultfd) continue;
                if (!(_rfds[i].events & POLLIN)) continue;
                if (_rfds[i].fd == _listensock && (_rfds[i].revents & POLLIN)) Accepter(_listensock);
                else if(_rfds[i].revents & POLLIN) Recver(i);
            }
        }
        void Print()
        {
            std::cout << "fd list ";
            for (int i = 0; i < num; i++)
            {
                if (_rfds[i].fd == defaultfd) continue;
                std::cout << _rfds[i].fd << " ";
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
                for (; i < num; i++)
                {
                    if (_rfds[i].fd != defaultfd) continue;
                    else break;
                }
                if (i == num)
                {
                    logMessage(WARNING, "server is full, please wait");
                    close(sock);
                }
                _rfds[i].fd = sock;
                _rfds[i].events = POLLIN;
                _rfds[i].revents = 0;
                Print();
        }
        void Recver(int pos)
        {
            // 1.读取request
            char buffer[1024];
            ssize_t s = recv(_rfds[pos].fd, buffer, sizeof(buffer) - 1, 0);
            if (s > 0)
            {
                buffer[s-2] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if (s == 0)
            {
                close(_rfds[pos].fd);
                ResetItem(pos);
                logMessage(NORMAL, "client quit");
                return;
            }
            else
            {
                close(_rfds[pos].fd);
                ResetItem(pos);
                logMessage(ERROR, "client quit: %s", strerror(errno));
                return;
            }

            // 2.处理request
            std::string response = _func(buffer);

            // 3.发送response  bug
            write(_rfds[pos].fd, response.c_str(), response.size());
        }
        void ResetItem(int pos)
        {
                _rfds[pos].fd = defaultfd;
                _rfds[pos].events = 0;
                _rfds[pos].revents = 0;
        }
    private:
        int _port;
        int _listensock;
        struct pollfd* _rfds;
        func_t _func;
    };
}
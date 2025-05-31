#pragma once

#include "sock.hpp"
#include <iostream>
#include <functional>
#include <string>
#include <sys/epoll.h>

namespace epoll_ns
{
    static const uint16_t defaultport = 8080;
    static const int size = 128;
    static const int defaultvalue = -1;
    static const int defaultnum = 64;

    using func_t = std::function<std::string(const std::string&)>;

    class EpollServer
    {
    public:
        EpollServer(func_t func, uint16_t port = defaultport, int num = defaultnum)
            :_func(func), _num(num), _port(port), _listensock(defaultvalue), _epfd(defaultvalue), _revs(nullptr)
        {}
        ~EpollServer()
        {
            if (_listensock != defaultvalue) close(_listensock);
            if (_epfd != defaultvalue) close(_epfd);
            if (_revs) delete[]_revs;
        }
        void initServer()
        {
            // 1.创建socket
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);

            // 2.创建epoll模型
            _epfd = epoll_create(size);
            if (_epfd < 0)
            {
                logMessage(FATAL, "epoll create error: %s", strerror(errno));
                exit(EPOLL_CREATE_ERR);
            }

            // 3.添加listensock到epoll中
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = _listensock;
            epoll_ctl(_epfd, EPOLL_CTL_ADD, _listensock, &ev);

            // 4.申请就绪事件的空间
            _revs = new struct epoll_event[_num];

            logMessage(NORMAL, "init server success");
        }
        void start()
        {
            int timeout = -1;
            for (;;)
            {
                int n = epoll_wait(_epfd, _revs, _num, timeout);
                if (n == 0) logMessage(NORMAL, "timeout...");
                else if (n == -1) logMessage(WARNING, "epoll_wait error, code: %d, err string: %s", errno, strerror(errno));
                else
                {
                    logMessage(NORMAL, "have event ready");
                    HandlerReadEvent(n);
                }
            }
        }
        void HandlerReadEvent(int readyNum)
        {
            logMessage(DEBUG, "HandlerReadEvent in");
            for (int i = 0; i < readyNum; i++)
            {
                uint32_t events = _revs[i].events;
                int sock = _revs[i].data.fd;

                if (sock == _listensock && (events & EPOLLIN)) Accepter();
                else if (events & EPOLLIN) Recver(sock);
                
            }
            logMessage(DEBUG, "HandlerReadEvent out");
        }
        void Accepter()
        {
            // listensock读事件就绪
            std::string clientip;
            uint16_t clientport;
            int sock = Sock::Accept(_listensock, &clientip, &clientport); 
            if (sock < 0) return;
            logMessage(NORMAL, "accept success [%s:%d]", clientip.c_str(), clientport);
            //将新的sock托管给epoll
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = sock;
            epoll_ctl(_epfd, EPOLL_CTL_ADD, sock, &ev);
        }
        void Recver(int sock)
        {
            // 1.读取request
            char buffer[1024];
            ssize_t s = recv(sock, buffer, sizeof(buffer) - 1, 0);
            // bug? 无法读取完整报文
            if (s > 0)
            {
                buffer[s - 2] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if (s == 0)
            {
                // 建议先epoll移除，再close
                epoll_ctl(_epfd, EPOLL_CTL_DEL, sock, nullptr);
                close(sock);
                logMessage(NORMAL, "client quit");
                return;
            }
            else
            {
                epoll_ctl(_epfd, EPOLL_CTL_DEL, sock, nullptr);
                close(sock);
                logMessage(ERROR, "recv errstring: %s", strerror(errno));
                return;
            }

            // 2.处理request
            std::string response = _func(buffer);

            // 3.发送response  bug
            write(sock, response.c_str(), response.size());
        }
        
    private:
        int16_t _port;
        int _listensock;
        int _epfd;
        func_t _func;
        struct epoll_event* _revs;
        int _num;
    };
}
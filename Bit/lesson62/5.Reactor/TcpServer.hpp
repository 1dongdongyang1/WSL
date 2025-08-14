#pragma once

#include "Sock.hpp"
#include "Epoller.hpp"
#include "Util.hpp"
#include "Protocol.hpp"
#include <iostream>
#include <functional>
#include <unordered_map>
#include <cassert>

namespace tcpserver
{
    class Connection;
    class TcpServer;

    static const uint16_t defaultport = 8080;
    static const int num = 64;

    using func_t = std::function<void(Connection*)>;
    // using handler_t = std::function<void(const std::string&)>;

    class Connection
    {
    public:
        Connection(int sock, TcpServer* tsp) :_sock(sock), _tsp(tsp) {}
        ~Connection() {}
        void Register(func_t r, func_t s, func_t e)
        {
            _recver = r;
            _sender = s;
            _excepter = e;
        }
        void Close()
        {
            close(_sock);
        }
    public:
        int _sock;
        std::string _inbuffer;   //输入缓冲区
        std::string _outbuffer;  //输出缓冲区

        func_t _recver;      //从sock读
        func_t _sender;      //从sock写
        func_t _excepter;    //处理sock_IO的异常事件

        TcpServer* _tsp;     // ?
    };

    class TcpServer
    {
    private:
        void Recver(Connection* conn)
        {
            char buffer[1024];
            while (true)
            {
                ssize_t s = recv(conn->_sock, buffer, sizeof(buffer) - 1, 0);
                if (s > 0)
                {
                    buffer[s] = 0;
                    conn->_inbuffer += buffer;
                }
                else if (s == 0)
                {
                    if (conn->_excepter)
                    {
                        conn->_excepter(conn);
                        return;
                    }
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                    else if (errno == EINTR) continue;
                    else
                    {
                        if (conn->_excepter)
                        {
                            conn->_excepter(conn);
                            return;
                        }
                    }
                }
            }
            _service(conn);
        }
        void Sender(Connection* conn)
        {
            while (true)
            {
                ssize_t s = send(conn->_sock, conn->_outbuffer.c_str(), conn->_outbuffer.size(), 0);
                if (s > 0)
                {
                    if (conn->_outbuffer.empty()) break;
                    else conn->_outbuffer.erase(0, s);
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                    else if (errno == EINTR) continue;
                    else
                    {
                        if (conn->_excepter)
                        {
                            conn->_excepter(conn);
                            return;
                        }
                    }
                }
            }
            // 如果没有发完, 就需要epoll关心, 发完了, 需要让epoll取消关心
            if (!conn->_outbuffer.empty()) EnableReadWrite(conn, true, true);
            else EnableReadWrite(conn, true, false);
        }
        void Excepter(Connection* conn)
        {
            // logMessage(DEBUG, "Excepter begin");
            _epoller.Control(conn->_sock, 0, EPOLL_CTL_DEL);
            conn->Close();
            _connections.erase(conn->_sock);
            // logMessage(DEBUG, "close %d resourse", conn->_sock);
            delete conn;
        }
        void Accepter(Connection* conn)
        {
            for (;;)
            {
                std::string clientip;
                uint16_t clientport;
                int err = 0;
                int sock = _sock.Accept(&clientip, &clientport, &err);
                if (sock > 0)
                {
                    AddConnection(sock, EPOLLIN | EPOLLET, \
                        std::bind(&TcpServer::Recver, this, std::placeholders::_1),
                        std::bind(&TcpServer::Sender, this, std::placeholders::_1),
                        std::bind(&TcpServer::Excepter, this, std::placeholders::_1));
                    logMessage(DEBUG, "get a new link, info:[%s:%d]", clientip.c_str(), clientport);
                }
                else
                {
                    if (err == EAGAIN || err == EWOULDBLOCK) break;
                    else if (err == EINTR) continue;
                    else break;
                }
            }
        }
        void AddConnection(int sock, uint32_t events, func_t recver, func_t sender, func_t excepter)
        {
            // 1.为该sock创建Connection, 并初始化, 并添加到Connections里
            if (events & EPOLLET) Util::SetNonBlock(sock);
            Connection* conn = new Connection(sock, this);
            conn->Register(recver, sender, excepter);

            // 2.添加到epoller里
            bool r = _epoller.AddEvent(sock, events);
            assert(r);
            (void)r;

            // 3.将kv添加到connections里
            _connections.insert(std::pair<int, Connection*>(sock, conn));
            logMessage(DEBUG, "add new sock: %d in epoll and unordered_map", sock);
        }
        bool IsConnectionExists(int sock)
        {
            auto iter = _connections.find(sock);
            return iter != _connections.end();
        }
        void Loop(int timeout)
        {
            int n = _epoller.Wait(_revs, _num, timeout);
            for (int i = 0; i < n; i++)
            {
                int sock = _revs[i].data.fd;
                uint32_t events = _revs[i].events;
                // 将异常问题转化成 读写问题
                if (events & EPOLLERR) events |= (EPOLLIN | EPOLLOUT);
                if (events & EPOLLHUP) events |= (EPOLLIN | EPOLLOUT);
                // listen事件就绪
                if ((events & EPOLLIN) && IsConnectionExists(sock) && _connections[sock]->_recver)
                    _connections[sock]->_recver(_connections[sock]);
                if ((events & EPOLLOUT) && IsConnectionExists(sock) && _connections[sock]->_sender)
                    _connections[sock]->_sender(_connections[sock]);
            }
        }
    public:
        TcpServer(func_t func, uint16_t port = defaultport) :_port(port), _revs(nullptr), _service(func)
        {
        }
        void InitServer()
        {
            // 1.创建socket
            _sock.Socket();
            _sock.Bind(_port);
            _sock.Listen();
            // 2.构建Epoll
            _epoller.Create();
            // 3.将_listensock添加到epoller里, 添加之前需要将fd设置为非阻塞
            AddConnection(_sock.Fd(), EPOLLIN | EPOLLET, \
                std::bind(&TcpServer::Accepter, this, std::placeholders::_1), nullptr, nullptr);

            _num = num;
            _revs = new struct epoll_event[_num];

        }
        void EnableReadWrite(Connection* conn, bool readable, bool writeable)
        {
            uint32_t events = (readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0) | EPOLLET;
            _epoller.Control(conn->_sock, events, EPOLL_CTL_MOD);
        }
        // 事件派发器
        void Dispatcher()
        {
            int timeout = -1;
            while (true)
            {
                Loop(timeout);
                logMessage(DEBUG, "time out...");
            }
        }
        ~TcpServer()
        {
            _sock.Close();
            _epoller.Close();
        }
    private:
        uint16_t _port;
        Sock _sock;
        Epoller _epoller;
        std::unordered_map<int, Connection*> _connections;
        struct epoll_event* _revs;
        int _num;
        // handler_t _handler;
        func_t _service;
    };
}

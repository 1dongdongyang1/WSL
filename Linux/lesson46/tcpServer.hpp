#pragma once

#include "log.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"
#include "daemon.hpp"
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
#include <pthread.h>

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

    

    class TcpServer
    {
    public:
        TcpServer(const uint16_t& port = gport)
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
        void start()
        {
            // 4.线程池初始化
            ThreadPool<Task>::getInstance()->run();
            logMessage(NORMAL, "Thread init success");
            // signal(SIGCHLD, SIG_IGN);   
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

                // 5.未来通信用sock,面向字节流的，后续全部是文件操作
                // version 1
                // serviceIO(sock);
                // close(sock);    //对于一个使用完的sock,我们要关闭这个sock,不然会导致文件描述符泄漏

                // version 2 多进程(1)
                // pid_t id = fork();
                // if (id == 0)    
                // {
                //     // child
                //     close(_listensock);
                //     if (fork() > 0) exit(0);
                //     // grandchild
                //     serviceIO(sock);
                //     close(sock);
                //     exit(0);
                // }
                // // parent
                // pid_t ret = waitpid(id, nullptr, 0);
                // if (ret > 0) cout << "wait success: " << ret << endl;
                // close(sock);


                // version 2 多进程(2)
                // pid_t id = fork();
                // if (id == 0)    
                // {
                //     // child
                //     close(_listensock);
                //     serviceIO(sock);
                //     close(sock);
                //     exit(0);
                // }
                // close(sock);

                // version 3 多线程版
                // pthread_t tid;
                // pthread_create(&tid, nullptr, threadRoutine, (void*)&sock);

                // version 4 线程池版
                ThreadPool<Task>::getInstance()->push(Task(sock, serviceIO));

            }
        }
        // static void* threadRoutine(void* args)
        // {
        //     pthread_detach(pthread_self());

        //     int sock = *(static_cast<int*>(args));
        //     serviceIO(sock);
        //     close(sock);
        //     return nullptr;
        // }

        ~TcpServer() {}
    private:
        int _listensock;  //不是用来通信的，用于监听链接到来
        uint16_t _port;
    };
}
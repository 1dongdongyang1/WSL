#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <strings.h>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

namespace Server
{
    using namespace std;

    typedef function<void(int, string, uint16_t, string)> fun_t;
    
    static const string defaultIp = "0.0.0.0";
    static const int gnum = 1024;

    enum {USAGE_ARR = 1, SOCKET_ERR, BIND_ERR, OPEN_ERR };

    class udpServer
    {
    public:
        udpServer(const fun_t& cb, uint16_t& port, const string& ip = defaultIp):_callback(cb), _port(port), _ip(ip),_sockfd(-1)
        {}
        void initServer()
        {
            // 1.创建socket
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sockfd == -1)
            {
                cerr << "socket error :" << errno << " : " << strerror(errno) << endl;
                exit(SOCKET_ERR);
            }
            cout << "create socket success: " << _sockfd << endl;
            // 2.绑定port, ip
            struct sockaddr_in local;
            bzero(&local, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);  //转大端
            local.sin_addr.s_addr = inet_addr(_ip.c_str());
            // local.sin_addr.s_addr = INADDR_ANY; //任意地址bind, 服务器的真实写法
            int n = bind(_sockfd, (struct sockaddr*)&local, sizeof(local));
            if (n == -1)
            {
                cerr << "bind error :" << errno << " : " << strerror(errno) << endl;
                exit(BIND_ERR);
            }
        }
        void start()
        {
            char buffer[gnum];
            for(;;)
            {
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);   //必填
                ssize_t s = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&peer, &len);
                if (s > 0)
                {
                    buffer[s] = 0;
                    string clientIp = inet_ntoa(peer.sin_addr);
                    uint16_t clientPort = ntohs(peer.sin_port);
                    string message = buffer;

                    cout << "Client" << clientIp << "[" << clientPort << "]#" << message << endl;
                    _callback(_sockfd, clientIp, clientPort, message);
                }
            }
        }

        ~udpServer()
        {

        }
    private:
        uint16_t _port;
        string _ip; 
        int _sockfd;
        fun_t _callback;
    };

}
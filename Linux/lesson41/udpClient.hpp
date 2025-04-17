#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <strings.h>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

namespace Client
{
    using namespace std;
    class udpClient
    {
    public:
        udpClient(const string& serverIp, const uint16_t& serverPort)
            :_serverIp(serverIp), _serverPort(serverPort), _sockfd(-1), _quit(false)
        {

        }
        void initClient()
        {
            // 创建socket
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sockfd == -1)
            {
                cerr << "socket error:" << errno << " : " << strerror(errno) << endl;
                exit(2);
            }
            cout << "create socket success: " << _sockfd << endl;
            // OS自动形成端口进行bind, 不需要程序员手动显式bind
        }
        void run()
        {
            struct sockaddr_in server;
            memset(&server, 0, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = inet_addr(_serverIp.c_str());
            server.sin_port = htons(_serverPort);

            string message;
            while (!_quit)
            {
                cout << "Please Enter# ";
                cin >> message;

                sendto(_sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&server, sizeof(server));
            }
        }
    private:
        int _sockfd;
        string _serverIp;
        uint16_t _serverPort;
        bool _quit;
    };
}
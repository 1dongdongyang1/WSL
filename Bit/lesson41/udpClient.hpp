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
#include <pthread.h>

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
        static void* readMessage(void* args)
        {
            int sockfd = *(static_cast<int*>(args));
            pthread_detach(pthread_self());

            while (true)
            {
                char buffer[1024];
                struct sockaddr_in tmp;
                socklen_t tmp_len = sizeof(tmp);   //必填
                ssize_t s = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&tmp, &tmp_len);
                // buffer_size是否-1, 取决于传入的是否是字符串
                if (s >= 0)
                {
                    buffer[s] = 0;
                    string serverIp = inet_ntoa(tmp.sin_addr);
                    uint16_t serverPort = ntohs(tmp.sin_port);
                    string message = buffer;

                    cout << serverIp << "[" << serverPort << "]#" << endl << message << endl;
                }
            }
            return nullptr;
        }

        void run()
        {
            pthread_create(&_reader, nullptr, readMessage, (void*)&_sockfd);

            struct sockaddr_in server;
            memset(&server, 0, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = inet_addr(_serverIp.c_str());
            server.sin_port = htons(_serverPort);

            string message;
            char cmdline[1024];
            while (!_quit)
            {
                cout << "Please Enter# ";
                // cin >> message;
                fgets(cmdline, sizeof(cmdline), stdin);
                message = cmdline;
                sendto(_sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&server, sizeof(server));
            }
        }
    private:
        int _sockfd;
        string _serverIp;
        uint16_t _serverPort;
        bool _quit;
        pthread_t _reader;
    };
}
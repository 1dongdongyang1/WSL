#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


namespace client
{
    using namespace std;
    class TcpClient
    {
    public:
        TcpClient(const string& serverip, const uint16_t& serverport)
            :_sock(-1), _serverip(serverip), _serverport(serverport)
        {}
        void initClient()
        {
            // 1.创建套接字
            _sock = socket(AF_INET, SOCK_STREAM, 0);
            if (_sock == -1)
            {
                cerr << "socket create err" << endl;
                exit(2);
            }
        }
        void start()
        {
            // 2.发起链接
            struct sockaddr_in server;
            memset(&server, 0, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = inet_addr(_serverip.c_str());
            server.sin_port = htons(_serverport);

            if(connect(_sock, (struct sockaddr*)&server, sizeof(server)) != 0)
            {
                cerr << "socket connect err" << endl;
            }
            else
            {
                string msg;
                while (true)
                {
                    cout << "Enter# ";
                    getline(cin, msg);
                    write(_sock, msg.c_str(), msg.size());

                    char buffer[1024];
                    int n = read(_sock, buffer, sizeof(buffer) - 1);
                    if (n > 0)
                    {
                        buffer[n] = 0;
                        cout << "Server回显# " << buffer << endl;
                    }
                    else break;
                }
            }
        }

        ~TcpClient()
        {
            if (_sock >= 0) close(_sock);
        }

    private:
        int _sock;
        string _serverip;
        uint16_t _serverport;
    };
}
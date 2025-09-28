#pragma once

#include "protocol.hpp"
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
    class CalClient
    {
    public:
        CalClient(const string& serverip, const uint16_t& serverport)
            :_sock(-1), _serverip(serverip), _serverport(serverport)
        {
        }
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

            if (connect(_sock, (struct sockaddr*)&server, sizeof(server)) != 0)
            {
                cerr << "socket connect err" << endl;
            }
            else
            {
                string line;
                string inbuffer;
                while (true)
                {
                    cout << "mycal>>> ";
                    getline(cin, line);
                    Request req = parseLine(line);

                    string content;
                    req.serialize(&content);
                    string send_string = enLength(content);
                    send(_sock, send_string.c_str(), send_string.size(), 0);

                    string package, text;
                    if (!recvPackage(_sock, inbuffer, &package)) continue;
                    if (!deLength(package, &text)) continue;
                    Response resp;
                    resp.deserialize(text);
                    cout << "exitcode: " << resp.exitcode << endl;
                    cout << "result: " << resp.result << endl;
                }
            }
        }

        Request parseLine(const string& line)
        {
            int status = 0;
            int i = 0;
            int cnt = line.size();
            string left, right;
            char op;
            while (i < cnt)
            {
                switch (status)
                {
                case 0:
                {
                    if (!isdigit(line[i]))
                    {
                        op = line[i];
                        status = 1;
                    }
                    else left.push_back(line[i++]);
                }
                break;
                case 1:
                    i++;
                    status = 2;
                    break;
                case 2:
                    right.push_back(line[i++]); 
                    break;
                }
            }
            // cout << left << ":" << right << ":" << op << endl;
            return Request(stoi(left), stoi(right), op);
        }

        ~CalClient()
        {
            if (_sock >= 0) close(_sock);
        }

    private:
        int _sock;
        string _serverip;
        uint16_t _serverport;
    };
}

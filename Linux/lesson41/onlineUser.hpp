#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <netinet/in.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

class User
{
public:
    User(const string& ip, const uint16_t& port) :_ip(ip), _port(port) {}
    ~User() {}
public:
    string _ip;
    uint16_t _port;
};

class OnlineUser
{
public:
    OnlineUser() {}
    ~OnlineUser() {}
    void addUser(const string& ip, const uint16_t& port)
    {
        string id = ip + "-" + to_string(port);
        _users.insert(make_pair(id, User(ip, port)));
    }
    void delUser(const string& ip, const uint16_t& port)
    {
        string id = ip + "-" + to_string(port);
        _users.erase(id);
    }
    bool isOnline(const string& ip, const uint16_t& port)
    {
        string id = ip + "-" + to_string(port);
        return _users.find(id) == _users.end() ? false : true;
    }
    void broadcastMessage(int sockfd, const string& ip, const uint16_t& port, const string& message)
    {
        for (auto& user : _users)
        {
            struct sockaddr_in client;
            memset(&client, 0, sizeof(client));
            client.sin_family = AF_INET;
            client.sin_addr.s_addr = inet_addr(user.second._ip.c_str());
            client.sin_port = htons(user.second._port);
            string s = ip + "-" + to_string(port) + "#";
            s += message;
            sendto(sockfd, s.c_str(), s.size(), 0, (struct sockaddr*)&client, sizeof(client));
        }
    }
private:
    unordered_map<string, User> _users;
};
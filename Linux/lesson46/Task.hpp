#pragma once

#include "log.hpp"
#include <iostream>
#include <functional>
#include <string>
#include <unistd.h>

void serviceIO(int sock)
{
    char buffer[1024];
    while (true)
    {
        ssize_t n = read(sock, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            // 目前我们把读到的数据当作字符串
            buffer[n] = 0;
            std::cout << "recv message: " << buffer << std::endl;

            std::string outbuffer = buffer;
            outbuffer += " server[echo]";

            write(sock, outbuffer.c_str(), outbuffer.size());   //多路转接
        }
        else if (n == 0)
        {
            // 代表client退出
            logMessage(NORMAL, "client quit, me too");
            break;
        }
    }
    close(sock);
}

class Task
{
public:
    using func_t = std::function<void(int)>;
public:
    Task() {}
    Task(int sock, func_t func) :_sock(sock), _callback(func)
    {
    }
    void operator()()
    {
        _callback(_sock);
    }
private:
    int _sock;
    func_t _callback;
};


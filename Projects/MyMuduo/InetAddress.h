#pragma once

#include <netinet/in.h>
#include <string>

// 封装socket地址类型
class InetAddress
{
public:
    InetAddress(uint16_t port = 8080, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in& addr);

    std::string toIp() const;
    uint16_t toPort() const;
    std::string toIpPort() const;

    const sockaddr_in* getSockAddr() const;
    void setSockAddr(const sockaddr_in& addr);
private:
    sockaddr_in addr_;
};
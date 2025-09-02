#pragma once

#include <netinet/in.h>
#include <string>

// 封装socket地址类型
class InetAddress
{
public:
    InetAddress(std::string ip, uint16_t port);
    explicit InetAddress(const sockaddr_in& addr);
    std::string toIp() const;
    uint16_t toPort() const;
    std::string toIpPort() const;
    const sockaddr_in* getSockAddr() const;
    void setSockAddr(const sockaddr_in& addr);
private:
    sockaddr_in addr_;
};
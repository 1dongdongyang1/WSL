#pragma once

#include "Buffer.h"
#include "Acceptor.h"
#include "Callbacks.h"
#include "EventLoop.h"
#include "noncopyable.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"

#include <atomic>
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

/**
 *                                   TcpServer -> loop
 *            ConnectionMap        EventLoopPool            Acceptor
 *            TcpConnection   ChannelList  poller
 *       buffer           channel          ChannelMap<fd,channel>
 *                      fd    回调
 */

// 对外的服务器编程使用的类
class TcpServer : noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    enum Option
    {
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& nameArg, Option option = kNoReusePort);
    ~TcpServer();

    void setThreadInitCallback(const ThreadInitCallback& cb) { threadInitCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

    // 设置底层subloop数量
    void setThreadNum(int numThreads);

    // 开启服务器监听
    void start();

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop* loop_;

    const std::string ipPort_;
    const std::string name_;

    std::unique_ptr<Acceptor> acceptor_;    // 运行在mianloop，负责监听新连接

    std::shared_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    ThreadInitCallback threadInitCallback_;

    std::atomic_int started_;

    int nextConnId_;
    ConnectionMap connections_;
};
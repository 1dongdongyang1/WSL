#pragma once

#include "CurrentThread.h"
#include "noncopyable.h"
#include "Timestamp.h"

#include <mutex>
#include <memory>
#include <atomic>
#include <vector>
#include <functional>

class Channel;
class Poller;

// 时间循环类  主要包含了 Channel 和 Poller(epoll的封装)
class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    // 开启事件循环
    void loop();
    // 退出事件循环
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    // 在当前线程中执行cb
    void runInloop(Functor cb);
    // 把cb放入队列中，唤醒loop所在线程，执行cb
    void queueInloop(Functor cb);

    // 用来唤醒loop所在的线程的
    void wakeup();

    // 调用Poller的相关方法
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    // 判断EventLoop的对象是否在自己的线程里
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
    void handleRead();          // wake up
    void doPendingFunctors();   // 执行回调

    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_;  // 原子操作，通过CAS实现
    std::atomic_bool quit_;     // 标识退出loop循环

    const pid_t threadId_;      // 记录loop当前所在线程id

    Timestamp pollReturnTime_;  // poller返回发生事件的channels的时间点
    std::unique_ptr<Poller> poller_;

    int wakeupFd_;              // 当mainloop获取一个新channel时，通过轮询算法选择一个subloop，让该subloop去接收处理
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;

    std::atomic_bool callingPendingFunctors_;   // 标识当前loop是否有需要执行回调的操作
    std::vector<Functor> pendingFunctors_;      // 存储loop需要执行的所有的回调操作
    std::mutex mutex_;                          // 互斥锁，用来保护上面vector容器的线程安全
};
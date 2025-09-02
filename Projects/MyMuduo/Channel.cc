#include "Channel.h"
#include "Logger.h"

#include <sys/epoll.h>

const int kNoneEvent = 0;
const int kReadEvent = EPOLLIN | EPOLLPRI;
const int kWriteEvent = EPOLLOUT;

// EventLoop -> channelList poller
Channel::Channel(EventLoop* loop, int fd)
    :loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false)
{}

Channel::~Channel() {}

// tie什么时候被调用
void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_ = true;
}

void Channel::remove()
{
    // add code
    // loop->removeChannel(this);
}

// EventLoop -> channelList poller channel通过eventloop调用poller的相关方法
void Channel::update()
{
    // add code
    // loop->updateChannel(this);
}

// poller通知EventLoop，EventLoop调用channel  与update一反
void Channel::handleEvent(Timestamp receiveTime)
{
    if (tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) handleEventWithGuard(receiveTime);
    }
    else handleEventWithGuard(receiveTime);
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    LOG_INFO("channel handleEvent revents:%d\n", revents_);

    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))  // 被动关闭方第三次挥手
    {
        if (closeCallback_) closeCallback_();
    }

    if(revents_ & EPOLLERR)
    {
        if (errorCallback_) errorCallback_();
    }

    if(revents_ & (EPOLLIN | EPOLLPRI))
    {
        if (readCallback_) readCallback_(receiveTime);
    }

    if(revents_ & EPOLLOUT)
    {
        if (writeCallback_) writeCallback_();
    }
}

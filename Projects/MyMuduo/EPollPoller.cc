#include "EPollPoller.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Logger.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>

// 未添加到Map和epoll里
const int kNew = -1;    // channel的index初始化为-1
// 添加到Map和epoll里
const int kAdded = 1;
// epoll里被删除，Map里还在
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop* loop)
    :Poller(loop)
    , epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize)   // vector<epoll_event>
{
    if (epollfd_ < 0) LOG_FATAL("epoll_create error:%d\n", errno);
}

EPollPoller::~EPollPoller() { ::close(epollfd_); }

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    // 应该是LOG_DEBUG
    LOG_INFO("func = %s -> fd total count:%ld\n", __FUNCTION__, channels_.size());

    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());

    if (numEvents > 0)
    {
        LOG_INFO("%d events happened\n", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == events_.size()) events_.resize(events_.size() * 2);
    }
    else if (numEvents == 0) LOG_DEBUG("%s timeout \n", __FUNCTION__);
    else
    {
        if (saveErrno != EINTR) LOG_ERROR("EPollPoller::poll() err\n");
    }
    return now;
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    for (int i = 0; i < numEvents; i++)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

// Channel: update remove -> EventLoop: updateChannel removeChannel -> Poller: updateChannel removeChannel
/**
 *                      EventLoop
 *          ChannelList         Poller
 *                              ChannelMap<fd, channel*>
 */
void EPollPoller::updateChannel(Channel* channel)
{
    const int index = channel->index();
    LOG_INFO("func = %s -> fd = %d  events = %d  index = %d\n",__FUNCTION__, channel->fd(), channel->events(), index);
    if(index == kNew || index == kDeleted)  // -> add
    {
        int fd = channel->fd();
        // index == kNew说明该channel没有在Map里注册过
        if (index == kNew) channels_[fd] = channel;
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else    // -> mod/del
    {
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);   // 只标记，没有从Map里删
        }
        else update(EPOLL_CTL_MOD, channel);
    }
}

// 从ChannelMap里删除
void EPollPoller::removeChannel(Channel* channel)
{
    int fd = channel->fd();
    channels_.erase(fd);
    LOG_INFO("func = %s -> fd = %d\n", __FUNCTION__, channel->fd());

    int index = channel->index();
    if (index == kAdded) update(EPOLL_CTL_DEL, channel);
    channel->set_index(kNew);
}

// add/mod/del
void EPollPoller::update(int operation, Channel* channel)
{
    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL) LOG_ERROR("epoll_ctl del error:%d\n", errno);
        else LOG_FATAL("epoll_ctl add/mod error:%d\n", errno);
    }
}
#pragma once

#include "Log.hpp"
#include "Err.hpp"
#include <iostream>
#include <sys/epoll.h>

static const int defaultepfd = -1;
static const int size = 128;

class Epoller
{
public:
    Epoller():_epfd(defaultepfd) {}
    ~Epoller()
    {
        Close();
    }
    void Close()
    {
        if (_epfd != defaultepfd) close(_epfd);
    }
public:
    void Create()
    {
        _epfd = epoll_create(size);
        if (_epfd < 0)
        {
            logMessage(FATAL, "epoll_create error, code: %d, errstring: %s", errno, strerror(errno));
            exit(EPOLL_CREATE_ERR);
        }
    }
    // user -> kernel
    bool AddEvent(int sock, uint32_t events)
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = sock;

        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, sock, &ev);
        return n == 0;
    }
    bool Control(int sock, uint32_t events, int action)
    {
        int n = 0;
        if (action == EPOLL_CTL_MOD)
        {
            struct epoll_event ev;
            ev.events = events;
            ev.data.fd = sock;
            n = epoll_ctl(_epfd, action, sock, &ev);
        }
        else if (action == EPOLL_CTL_DEL) n = epoll_ctl(_epfd, action, sock, nullptr);
        else n = -1;
        return n == 0;
    }

    // kernel -> user
    int Wait(struct epoll_event revs[], int num, int timeout)
    {
        int n = epoll_wait(_epfd, revs, num, timeout);
        return n;
    }

private:
    int _epfd;
};
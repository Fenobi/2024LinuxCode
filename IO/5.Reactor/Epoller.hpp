#pragma once

#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

#include "Err.hpp"
#include "Log.hpp"

static const int defaultepfd = -1;
static const int size = 128;

class Epoller
{
public:
    Epoller():epfd_(defaultepfd)
    {}
    
    ~Epoller()
    {
        if(epfd_!=defaultepfd)
            close(epfd_);
    }
public:
    void Create()
    {
        epfd_ = epoll_create(size);
        if(epfd_<0)
        {
            logMessage(FATAL, "epoll_create error code: %d, errstring: %s", errno, strerror(errno));
        }
    }

    bool AddEvent(int sock,uint32_t events)
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = sock;

        int n = epoll_ctl(epfd_, EPOLL_CTL_ADD, sock, &ev);
        return n == 0;
    }

    int wait()
    {
        return 0;
    }

private:
    int epfd_;
};
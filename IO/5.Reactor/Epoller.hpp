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
    Epoller() : epfd_(defaultepfd)
    {
    }

    ~Epoller()
    {
        if (epfd_ != defaultepfd)
            close(epfd_);
    }

public:
    void Create()
    {
        epfd_ = epoll_create(size);
        if (epfd_ < 0)
        {
            logMessage(FATAL, "epoll_create error code: %d, errstring: %s", errno, strerror(errno));
            exit(EPOLL_CREATE_ERR);
        }
    }

    bool AddEvent(int sock, uint32_t events)
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = sock;

        int n = epoll_ctl(epfd_, EPOLL_CTL_ADD, sock, &ev);
        return n == 0;
    }

    int Wait(struct epoll_event revs[], int num, int timeout)
    {
        int n = epoll_wait(epfd_, revs, num, timeout);
        return n;
    }

    bool Control(int sock,uint32_t event, int action)
    {
        int n = 0;
        if (action == EPOLL_CTL_MOD)
        {
            struct epoll_event ev;
            ev.events = event;
            ev.data.fd=sock;
            n = epoll_ctl(epfd_, action, sock, &ev);
        }
        else if(action == EPOLL_CTL_DEL)
        {
            n= epoll_ctl(epfd_, action, sock, nullptr);
        }
        else
            n = -1;
        return n == 0;
    }

    void Close()
    {
        if (epfd_ != defaultepfd)
            close(epfd_);
    }

private:
    int epfd_;
};
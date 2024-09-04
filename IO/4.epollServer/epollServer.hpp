#pragma once
#include "sock.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <memory>
#include <sys/epoll.h>
#include <cstring>
#include <cerrno>

using func_t = std::function<std::string(const std::string &)>;

namespace epoll_ns
{
    static const int defaultport = 8080;
    static const int size = 128;
    static const int defaultnum = 64;
    static const int defaultvalue = -1;

    class EpollServer
    {
    public:
        EpollServer(func_t func, uint16_t port = defaultport, int num = defaultnum)
            : _func(func), _port(port), _listensock(-1), _num(defaultnum), _revs(nullptr), _epfd(-1)
        {
        }

        // void ReseItem(int i)
        // {
        //     _rfds[i].fd = defaultfd;
        //     _rfds[i].events = 0;
        //     _rfds[i].revents = 0;
        // }

        void initServer()
        {
            // 1. 创建socket
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);
            // 2. 创建epoll模型
            _epfd = epoll_create(size);
            if (_epfd < 0)
            {
                logMessage(FATAL, "epoll create error: %s", strerror(errno));
                exit(EPOLL_CREATE_ERR);
            }

            // 3.添加listensock到epoll中
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = _listensock; // 当事件就绪，被重新捞上来的时候，我们要知道哪个fd就绪了
            epoll_ctl(_epfd, EPOLL_CTL_ADD, _listensock, &ev);

            // 4.申请就绪事件的空间
            _revs = new struct epoll_event[_num];
            logMessage(NORMAL, "epoll init success");
        }

        // void Print()
        // {
        //     std::cout << "fd list : ";
        //     for (int i = 0; i < num; ++i)
        //         if (_rfds[i].fd != defaultfd)
        //             std::cout << _rfds[i].fd << " ";
        //     std::cout << std::endl;
        // }

        // void Accepter(int listensock)
        // {
        //     // 读就绪
        //     std::string clientip;
        //     uint16_t clientport = 0;
        //     int sock = Sock::Accept(listensock, &clientip, &clientport); // 此时不会阻塞
        //     if (sock < 0)
        //         return;
        //     logMessage(NORMAL, "accept success[%s:%d]", clientip.c_str(), clientport);
        //     // 将新的sock托管给select
        //     // 本质：就是将sock添加到fdarry数组中即可
        //     int i = 0;
        //     for (; i < num; ++i)
        //     {
        //         if (_rfds[i].fd != defaultfd)
        //             continue;
        //         else
        //             break;
        //     }
        //     if (i == num)
        //     {
        //         logMessage(WARNING, "server is full,please wait!");
        //         close(sock);
        //     }
        //     else
        //     {
        //         _rfds[i].fd = sock;
        //         _rfds[i].events = POLLIN;
        //         _rfds[i].revents = 0;
        //     }
        //     Print();
        // }

        // void Recver(int pos)
        // {
        //     // 读取有问题
        //     char buffer[1024];
        //     ssize_t s = recv(_rfds[pos].fd, buffer, sizeof(buffer) - 1, 0); // 不会被堵塞
        //     if (s > 0)
        //     {
        //         buffer[s] = 0;
        //         logMessage(NORMAL, "client# %s", buffer);
        //     }
        //     else if (s == 0)
        //     {
        //         close(_rfds[pos].fd);
        //         ReseItem(pos);
        //         logMessage(NORMAL, "client quit");
        //         return;
        //     }
        //     else
        //     {
        //         close(_rfds[pos].fd);
        //         ReseItem(pos);
        //         logMessage(ERROR, "client quit: %s", strerror(errno));
        //         return;
        //     }
        //     std::string response = _func(buffer);
        //     // write
        //     write(_rfds[pos].fd, response.c_str(), response.size());
        // }

        void HandlerReadEvent(int readyNum)
        {
            logMessage(NORMAL, "HandlerReadEvent in");
            for (int i = 0; i < readyNum; ++i)
            {
                uint32_t events = _revs[i].events;
                int sock = _revs[i].data.fd;
                if (sock == _listensock && (events & EPOLLIN))
                {
                    //_listensock读事件就绪，获取新链接
                    std::string clientip;
                    uint16_t clientport;
                    int fd = Sock::Accept(sock, &clientip, &clientport);
                    if (fd < 0)
                    {
                        logMessage(WARNING, "accept error");
                        continue;
                    }
                    // 获取成功，可以直接取吗？不可以，放入epoll
                    struct epoll_event ev;
                    ev.events = EPOLLIN;
                    ev.data.fd = fd;
                    epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
                }
                else if (events & EPOLLIN)
                {
                    // 普通的读事件
                    // 依旧有问题
                    char buffer[1024];
                    int n = recv(sock, buffer, sizeof buffer, 0);
                    if (n > 0)
                    {
                        buffer[n] = 0;
                        logMessage(DEBUG, "client# %s", buffer);

                        std::string response = _func(buffer);
                        send(sock, response.c_str(), response.size(), 0);
                    }
                    else if (n == 0)
                    {
                        // 先移除epoll，再close fd。
                        epoll_ctl(_epfd, EPOLL_CTL_DEL, sock, nullptr);
                        close(sock);
                        logMessage(NORMAL, "epoll quit");
                    }
                    else
                    {
                        epoll_ctl(_epfd, EPOLL_CTL_DEL, sock, nullptr);
                        close(sock);
                        logMessage(ERROR, "recv error, code: %d, errstring: %s", errno, strerror(errno));
                    }
                }
                else
                {
                }
            }
            logMessage(DEBUG, "HandlerReadEvent out");
        }

        void statr()
        {
            int timeout = -1;
            for (;;)
            {
                int n = epoll_wait(_epfd, _revs, _num, timeout);
                switch (n)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "epoll_wait failed, code: %d, err string: %s", errno, strerror(errno));
                    break;

                default:
                    // 说明有事件就绪了，目前只有一个监听事件
                    logMessage(NORMAL, "have event ready");
                    HandlerReadEvent(n);
                    break;
                }

                // std::string clientip;
                // uint16_t clientport = 0;
                // int sock = Sock::Accept(_listensock, &clientip, &clientport);
                // if(sock<0)continue;
                // to do
            }
        }
        ~EpollServer()
        {
            if (_listensock != defaultvalue)
                close(_listensock);
            if (_epfd)
                close(_epfd);
            if (_revs)
                delete[] _revs;
        }

    private:
        int _port;
        int _listensock;
        int _epfd;
        struct epoll_event *_revs;
        int _num;
        func_t _func;
    };
}

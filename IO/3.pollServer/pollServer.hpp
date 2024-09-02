#pragma once
#include "sock.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <memory>
#include <poll.h>
#include <cstring>
#include <cerrno>

using func_t = std::function<std::string(const std::string &)>;

namespace poll_ns
{
    static const int defaultport = 8080;
    static const int num = 2048;
    static const int defaultfd = -1;

    class PollServer
    {
    public:
        PollServer(func_t f, int port = defaultport)
            : _func(f), _port(port), _listensock(-1), _rfds(nullptr)
        {
        }

        void ReseItem(int i)
        {
            _rfds[i].fd = defaultfd;
            _rfds[i].events = 0;
            _rfds[i].revents = 0;
        }

        void initServer()
        {
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);

            _rfds = new struct pollfd[num];
            for (int i = 0; i < num; ++i)
            {
                ReseItem(i);
            }
            _rfds[0].fd = _listensock;
            _rfds[0].events = POLLIN;
        }

        void Print()
        {
            std::cout << "fd list : ";
            for (int i = 0; i < num; ++i)
                if (_rfds[i].fd != defaultfd)
                    std::cout << _rfds[i].fd << " ";
            std::cout << std::endl;
        }

        void Accepter(int listensock)
        {
            // 读就绪
            std::string clientip;
            uint16_t clientport = 0;
            int sock = Sock::Accept(listensock, &clientip, &clientport); // 此时不会阻塞
            if (sock < 0)
                return;
            logMessage(NORMAL, "accept success[%s:%d]", clientip.c_str(), clientport);
            // 将新的sock托管给select
            // 本质：就是将sock添加到fdarry数组中即可
            int i = 0;
            for (; i < num; ++i)
            {
                if (_rfds[i].fd != defaultfd)
                    continue;
                else
                    break;
            }
            if (i == num)
            {
                logMessage(WARNING, "server is full,please wait!");
                close(sock);
            }
            else
            {
                _rfds[i].fd = sock;
                _rfds[i].events = POLLIN;
                _rfds[i].revents = 0;
            }
            Print();
        }

        void Recver(int pos)
        {
            // 读取有问题
            char buffer[1024];
            ssize_t s = recv(_rfds[pos].fd, buffer, sizeof(buffer) - 1, 0); // 不会被堵塞
            if (s > 0)
            {
                buffer[s] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if (s == 0)
            {
                close(_rfds[pos].fd);
                ReseItem(pos);
                logMessage(NORMAL, "client quit");
                return;
            }
            else
            {
                close(_rfds[pos].fd);
                ReseItem(pos);
                logMessage(ERROR, "client quit: %s", strerror(errno));
                return;
            }
            std::string response = _func(buffer);
            // write
            write(_rfds[pos].fd, response.c_str(), response.size());
        }

        void HandlerReadEvent()
        {
            for (int i = 0; i < num; ++i)
            {
                if (_rfds[i].fd == defaultfd)
                    continue;
                if (!(_rfds[i].revents & POLLIN))
                    continue;
                if (_rfds[i].fd == _listensock && (_rfds[i].revents & POLLIN))
                {
                    Accepter(_rfds[i].fd);
                }
                else if (_rfds[i].events & POLLIN)
                {
                    Recver(i);
                }
                else
                {
                }
            }
        }

        void statr()
        {
            for (;;)
            {
                int timeout = 1000;
                int n = poll(_rfds, num, timeout);
                switch (n)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "select error, code: %d, err string: %s", errno, strerror(errno));
                    break;

                default:
                    // 说明有事件就绪了，目前只有一个监听事件
                    logMessage(NORMAL, "have event ready!...");
                    HandlerReadEvent();
                    break;
                }

                // std::string clientip;
                // uint16_t clientport = 0;
                // int sock = Sock::Accept(_listensock, &clientip, &clientport);
                // if(sock<0)continue;
                // to do
            }
        }
        ~PollServer()
        {
            if (_listensock < 0)
                close(_listensock);
            if (_rfds)
                delete[] _rfds;
        }

    private:
        int _port;
        int _listensock;
        struct pollfd *_rfds;
        func_t _func;
    };
}

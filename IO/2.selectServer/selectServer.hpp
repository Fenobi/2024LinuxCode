#pragma once
#include "sock.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <memory>
#include <cstring>
#include <cerrno>

using func_t = std::function<std::string(const std::string &)>;

namespace select_ns
{
    static const int defaultport = 8080;
    static const int fd_num = sizeof(fd_set) * 8;
    static const int defaultfd = -1;

    class SelectServer
    {
    public:
        SelectServer(func_t f, int port = defaultport)
            : _func(f), _port(port), _listensock(-1), _fdarry(nullptr)
        {
        }

        void initServer()
        {
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);

            _fdarry = new int[fd_num];
            for (int i = 0; i < fd_num; ++i)
            {
                _fdarry[i] = defaultfd;
            }
            _fdarry[0] = _listensock;
        }
        
        void Print()
        {
            std::cout << "fd list : ";
            for (int i = 0; i < fd_num; ++i)
                if (_fdarry[i] != defaultfd)
                    std::cout << _fdarry[i] << " ";
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
            for (; i < fd_num; ++i)
            {
                if (_fdarry[i] != defaultfd)
                    continue;
                else
                    break;
            }
            if (i == fd_num)
            {
                logMessage(WARNING, "server is full,please wait!");
                close(sock);
            }
            else
            {
                _fdarry[i] = sock;
            }
            Print();
        }

        void Recver(int sock, int pos)
        {
            // 读取有问题
            char buffer[1024];
            ssize_t s = recv(sock, buffer, sizeof(buffer) - 1, 0); // 不会被堵塞
            if (s > 0)
            {
                buffer[s] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if (s == 0)
            {
                close(sock);
                _fdarry[pos] = defaultfd;
                logMessage(NORMAL, "client quit");
                return;
            }
            else
            {
                close(sock);
                _fdarry[pos] = defaultfd;
                logMessage(ERROR, "client quit: %s", strerror(errno));
                return;
            }
            std::string response = _func(buffer);
            // write
            write(sock, response.c_str(), response.size());
        }

        void HandlerReadEvent(fd_set &rfds)
        {
            for (int i = 0; i < fd_num; ++i)
            {
                if (_fdarry[i] == defaultfd)
                    continue;
                if (FD_ISSET(_fdarry[i], &rfds) && _fdarry[i] == _listensock)
                {
                    Accepter(_listensock);
                }
                else if (FD_ISSET(_fdarry[i], &rfds))
                {
                    Recver(_fdarry[i], i);
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
                fd_set rfds;
                FD_ZERO(&rfds);
                int maxfd = _fdarry[0];
                for (int i = 0; i < fd_num; ++i)
                {
                    if (_fdarry[i] == defaultfd)
                        continue;
                    FD_SET(_fdarry[i], &rfds); // 合法fd添加到文件描述符集中
                    if (maxfd < _fdarry[i])
                        maxfd = _fdarry[i]; // 更新所有fd中最大的fd
                }

                // struct timeval timeout = {2, 0}; // 输入输出型
                int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
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
                    HandlerReadEvent(rfds);
                    break;
                }

                // std::string clientip;
                // uint16_t clientport = 0;
                // int sock = Sock::Accept(_listensock, &clientip, &clientport);
                // if(sock<0)continue;
                // to do
            }
        }
        ~SelectServer()
        {
            if (_listensock < 0)
                close(_listensock);
            if (_fdarry)
                delete[] _fdarry;
        }

    private:
        int _port;
        int _listensock;
        int *_fdarry;
        func_t _func;
    };
}

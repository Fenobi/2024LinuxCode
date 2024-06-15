#pragma once

#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <signal.h>
#include <unordered_map>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <functional>

#include "Protocol.hpp"
#include "Util.hpp"

namespace server
{
    enum
    {
        USAGE_ERR = 1,
        SOCKET_ERR,
        LISTEN_ERR
    };

    static const uint16_t gport = 8080;
    static const int gbacklog = 5;

    using func_t = std::function<void(const httpRequest &, httpResponse &)>;

    class httpServer
    {

    public:
        httpServer(func_t func, const uint16_t &port = gport)
            : _func(func), _listensock(-1), _port(port)
        {
        }

        void initServer()
        {
            // 1.创建socket套接字
            _listensock = socket(AF_INET, SOCK_STREAM, 0);
            if (_listensock < 0)
            {
                exit(SOCKET_ERR);
            }

            // 2.bind绑定自己的网络信息
            struct sockaddr_in local;
            memset(&local, 0, sizeof local);
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if (bind(_listensock, (struct sockaddr *)&local, sizeof local) < 0)
            {
                exit(SOCKET_ERR);
            }

            // 3.设置socket为监听状态
            if (listen(_listensock, gbacklog) < 0)
            {
                exit(LISTEN_ERR);
            }
        }

        void registerCb(std::string servicename,func_t cb)
        {
            funcs.insert(std::make_pair(servicename, cb));
        }

        void HandlerHttp(int sock)
        {
            // 1.读到完整的http请求
            // 2.反序列化
            // 3.httprequst,httpresponse,_func(req,resq)
            // 4.resp序列化
            // 5.send
            char buffer[4096];
            httpRequest req;
            httpResponse resp;
            size_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] = 0;
                req.inbuffer = buffer;
                req.parse();
                //funcs[req.path](req, resp);
                _func(req, resp);
                send(sock, resp.outbuffer.c_str(), resp.outbuffer.size(), 0);
            }
        }

        void start()
        {
            for (;;)
            {
                // 4.server获取新链接
                // sock，和client进行通信的fd
                struct sockaddr_in peer;
                socklen_t len = sizeof peer;
                int sock = accept(_listensock, (struct sockaddr *)&peer, &len);
                if (sock < 0)
                {
                    continue;
                }
                // 5.这里就是一个sock，未来通信就用这个sock，面向字节流的，后续全部都是文件操作

                // version 2,多进程版
                pid_t id = fork();
                if (id == 0)
                {
                    close(_listensock);
                    if (fork() > 0)
                        exit(0);
                    HandlerHttp(sock);
                    close(sock);
                    exit(0);
                }
                close(sock);
                // father
                pid_t ret = waitpid(id, nullptr, 0);
            }
        }

        ~httpServer() {}

    private:
        int _listensock; // 不是用来数据通信的，它是用来监听链接到来，获取新链接的！
        uint16_t _port;
        func_t _func;
        std::unordered_map < std::string, func_t > funcs;
    };
}
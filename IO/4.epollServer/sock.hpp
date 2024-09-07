#pragma once

#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <functional>

#include "log.hpp"
#include "err.hpp"


static const int gbacklog = 5;

class Sock
{
public:
    static int Socket()
    {
        // 1.创建socket套接字
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            logMessage(FATAL, "create socket error");
            exit(SOCKET_ERR);
        }
        logMessage(NORMAL, "create socket success: %d", sock);
        return sock;
    }

    static void Bind(int sock, int port)
    {
        // 2.bind绑定自己的网络信息
        struct sockaddr_in local;
        memset(&local, 0, sizeof local);
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        if (bind(sock, (struct sockaddr *)&local, sizeof local) < 0)
        {
            logMessage(FATAL, "bind socket error");
            exit(SOCKET_ERR);
        }
        logMessage(NORMAL, "bind socket success");
    }

    static void Listen(int sock)
    {
        // 3.设置socket为监听状态
        if (listen(sock, gbacklog) < 0)
        {
            logMessage(FATAL, "listen socket error");
            exit(LISTEN_ERR);
        }
        logMessage(NORMAL, "listen socket success");
    }

    static int Accept(int listensock, std::string *clientip, uint16_t *clientport)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof peer;
        int sock = accept(listensock, (struct sockaddr *)&peer, &len);
        if (sock < 0)
        {
            logMessage(ERROR, "accept error,next");
        }
        else
        {
            logMessage(NORMAL, "accept a new link success,get new sock: %d", sock);
            *clientip = inet_ntoa(peer.sin_addr);
            *clientport = ntohs(peer.sin_port);
        }

        return sock;
    }
};
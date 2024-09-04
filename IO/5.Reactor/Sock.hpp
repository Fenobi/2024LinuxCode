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

#include "Log.hpp"
#include "Err.hpp"


static const int gbacklog = 5;
static const int defaultsock = -1;

class Sock
{
public:
    Sock():_listensock(defaultsock)
    {}
    int Fd()
    {
        return _listensock;
    }
    ~Sock()
    {
        if(_listensock!=defaultsock)
            close(_listensock);
    }
    void Socket()
    {
        // 1.创建socket套接字
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensock < 0)
        {
            logMessage(FATAL, "create socket error");
            exit(SOCKET_ERR);
        }
        logMessage(NORMAL, "create socket success: %d", _listensock);
    }

    void Bind(int port)
    {
        // 2.bind绑定自己的网络信息
        struct sockaddr_in local;
        memset(&local, 0, sizeof local);
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        if (bind(_listensock, (struct sockaddr *)&local, sizeof local) < 0)
        {
            logMessage(FATAL, "bind socket error");
            exit(SOCKET_ERR);
        }
        logMessage(NORMAL, "bind socket success");
    }

    void Listen()
    {
        // 3.设置socket为监听状态
        if (listen(_listensock, gbacklog) < 0)
        {
            logMessage(FATAL, "listen socket error");
            exit(LISTEN_ERR);
        }
        logMessage(NORMAL, "listen socket success");
    }

    int Accept(std::string *clientip, uint16_t *clientport)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof peer;
        int sock = accept(_listensock, (struct sockaddr *)&peer, &len);
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

        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof opt);
        return sock;
    }
private:
    int _listensock;
};
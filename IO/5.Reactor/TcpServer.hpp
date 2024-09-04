#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include <cassert>
#include "Sock.hpp"
#include "Epoller.hpp"
#include "Util.hpp"

namespace tcpserver
{
    static const int defaultsock = -1;
    static const int defaultport = 8080;
    class Connection;
    class TcpServer;

    using func_t = std::function<void(Connection *)>;

    class Connection
    {
    public:
        Connection(int sock)
            : sock_(sock)
        {
        }

        void Register(func_t r, func_t s, func_t e)
        {
            func_t recver_ = r;
            func_t sender_ = s;
            func_t excepter_ = e;
        }

        ~Connection()
        {
        }

    public:
        int sock_;
        std::string inbuffer_;
        std::string outbuffer_;

        func_t recver_;   // 从sock读
        func_t sender_;   // 向sock写
        func_t excepter_; // 处理sick io的时候上面的异常

        TcpServer *tsp;
    };

    class TcpServer
    {
    public:
        TcpServer(uint16_t port)
            : port_(defaultport)
        {
        }

        void InitServer()
        {
            // 1、构建socket
            sock_.Socket();
            sock_.Bind(port_);
            sock_.Listen();
            // 2、构建Epoll
            epoller_.Create();
            // 3、将目前唯一的sock，添加到epoller中
            Util::SetNonBlock(sock_.Fd());

            AddConnection(sock_.Fd(), EPOLLET | EPOLLIN,
             std::bind(&TcpServer::Accepter,this,std::placeholders::_1), nullptr, nullptr);
        }

        void Accepter(Connection* conn)
        {

        }

        void AddConnection(int sock, uint32_t events, func_t recver, func_t sender, func_t excepter)
        {
            // 1.首先要为该sock创建Connection，并初始化，并添加到connections中
            if (events & EPOLLET)
                Util::SetNonBlock((sock));
            Connection *conn = new Connection(sock);

            // 2.给对应的sock设置对应的回调方法
            conn->Register(recver, sender, excepter);

            // 3.其次将sock与它要关心的事件“写透式”注册到epoll中
            bool r = epoller_.AddEvent(sock_.Fd(), events);
            assert(r);
            (void)r;

            // 将kv添加到connections中
            connections_.insert(std::pair<int, Connection *>(sock, conn));
        }

        void Dispatch()
        {
            while (true)
            {
                int n = epoller_.wait();
                for (int i = 0; i < n;++i)
                {
                    int sock = 1;
                    connections_[sock]->inbuffer_ = 1;
                }
            }
        }

        ~TcpServer()
        {
        }

    private:
        uint16_t port_;
        Sock sock_;
        Epoller epoller_;
        std::unordered_map<int, Connection *> connections_;
    };
}
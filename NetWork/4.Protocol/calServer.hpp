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
#include "Protocol.hpp"

namespace server
{
    enum {
        USAGE_ERR = 1,
        SOCKET_ERR,
        LISTEN_ERR
    };

    static const uint16_t gport = 8080;
    static const int gbacklog = 5;

    // const Request &req:输入型
    // Response &resq:输出型
    typedef std::function<bool(const Request &req, Response &resp)> func_t;

    //保证解耦
    void handlerEntery(int sock, func_t func)
    {
        //一、读取 "content_len"\r\n"x op y"\r\n
        //1.如何保证读取的是【一个】完整数据
        std::string req_text;
        if(!recvRequest(sock, &req_text))
            return;
        //2、我们保证，我们req_text里面一定是个完整的请求

        std::string req_str;

        // 二、对请求Request，反序列化
        // 1、得到一个结构化的请求对象
        Request req;
        if(!req.deserialize(req_str))
            return;
        
        // 三、计算处理，req,x req,op req,y
        // 1.得到一个结构化的响应
        Response resp;
        func(req, resp);// req的处理结果全部放入resp中，回调

        // 四、对响应Response，进行序列化
        // 1.得到一个“字符串”
        std::string resp_str;
        resp.serialize(&resp_str);
        // 五、发送响应
        //1、构建成为一个完整的报文
        std::string send_string = enLength(resp_str);
        send(sock, send_string.c_str(), send_string.size(), 0);//发送有问题

    }

    class TcpServer
    {

    public:
        TcpServer(const uint16_t& port = gport)
            :_listensock(-1), _port(port)
        {}

        void initServer()
        {
            //1.创建socket套接字
            _listensock = socket(AF_INET, SOCK_STREAM, 0);
            if (_listensock < 0)
            {
                logMessage(FATAL, "create socket error");
                exit(SOCKET_ERR);
            }
            logMessage(NORMAL, "create socket success: %d",_listensock);

            //2.bind绑定自己的网络信息
            struct sockaddr_in local;
            memset(&local, 0, sizeof local);
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if (bind(_listensock, (struct sockaddr*)&local, sizeof local) < 0)
            {
                logMessage(FATAL, "bind socket error");
                exit(SOCKET_ERR);
            }
            logMessage(NORMAL, "bind socket success");

            //3.设置socket为监听状态
            if (listen(_listensock, gbacklog) < 0)
            {
                logMessage(FATAL, "listen socket error");
                exit(LISTEN_ERR);
            }
            logMessage(NORMAL, "listen socket success");


        }

        void start(func_t func)
        {
            for (;;)
            {
                //4.server获取新链接
                //sock，和client进行通信的fd
                struct sockaddr_in peer;
                socklen_t len = sizeof peer;
                int sock = accept(_listensock, (struct sockaddr*)&peer, &len);
                if (sock < 0)
                {
                    logMessage(ERROR, "accept error,next");
                    continue;
                }
                logMessage(NORMAL, "accept a new link success,get new sock: %d", sock);
                //5.这里就是一个sock，未来通信就用这个sock，面向字节流的，后续全部都是文件操作
         
                //version 2,多进程版
                pid_t id = fork();
                if (id == 0)
                {
                    close(_listensock);
                    // if (fork() > 0)exit(0);
                    //serviceIO(sock);
                    handlerEntery(sock,func);
                    close(sock);
                    exit(0);
                }
                close(sock);
                //father
                pid_t ret = waitpid(id, nullptr, 0);
                if (ret > 0)
                {
                    logMessage(NORMAL, "wait child success");
                }
            }
        }

        ~TcpServer() {}
    private:
        int _listensock;//不是用来数据通信的，它是用来监听链接到来，获取新链接的！
        uint16_t _port;

    };
}
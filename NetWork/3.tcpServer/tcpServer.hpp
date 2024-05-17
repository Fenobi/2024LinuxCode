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

#include "log.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"

namespace server
{
    enum {
        USAGE_ERR = 1,
        SOCKET_ERR,
        LISTEN_ERR
    };

    static const uint16_t gport = 8080;
    static const int gbacklog = 5;
    class TcpServer;

    class ThreadData
    {
    public:
        ThreadData(TcpServer* self, int sock)
            : _self(self), _sock(sock)
        {}
    public:
        TcpServer* _self;
        int _sock;
    };

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

        void start()
        {
            //线程池初始化
            ThreadPool<Task>::getInstance()->run();
            logMessage(NORMAL, "Thread init success");
            //signal(SIGCHLD, SIG_IGN);
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
                logMessage(ERROR, "3: %d", sock);
                logMessage(WARNING, "4: %d", sock);
                logMessage(FATAL, "5: %d", sock);
                std::cout << "sock: " << sock << std::endl;

                // //5.这里就是一个sock，未来通信就用这个sock，面向字节流的，后续全部都是文件操作
                // //version 1
                // serviceIO(sock);
                // close(sock);//对于一个使用完毕的sock，我们要关闭这个sock，不然会导致,文件描述符泄露

                // //version 2,多进程版
                // pid_t id = fork();
                // if (id == 0)
                // {
                //     close(_listensock);
                //     // if (fork() > 0)exit(0);
                //     serviceIO(sock);
                //     close(sock);
                //     exit(0);
                // }
                // close(sock);
                // // //father
                // // pid_t ret = waitpid(id, nullptr, 0);
                // // if (ret > 0)
                // // {
                // //     std::cout << "wait success: " << ret << std::endl;
                // // }

                // //version 3多线程版
                // pthread_t tid;
                // ThreadData* td = new ThreadData(this, sock);
                // pthread_create(&tid, nullptr, threadRoutine, td);

                // // pthread_join(tid, nullptr);

                //version 4线程池版
                
                ThreadPool<Task>::getInstance()->push(Task(sock, serviceIO));

            }
        }

        static void* threadRoutine(void* args)
        {
            pthread_detach(pthread_self());
            ThreadData* td = static_cast<ThreadData*>(args);
            serviceIO(td->_sock);
            close(td->_sock);
            delete td;
            return nullptr;
        }

        ~TcpServer() {}
    private:
        int _listensock;//不是用来数据通信的，它是用来监听链接到来，获取新链接的！
        uint16_t _port;

    };
}
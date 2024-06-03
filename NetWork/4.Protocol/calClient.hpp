#pragma once

#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Protocol.hpp"

#define NUM 1024

class CalClient
{
public:
    CalClient(const std::string &serverip, const uint16_t &serverport)
        : _sock(-1), _serverip(serverip), _serverport(serverport)
    {}

    void initClient()
    {
        //1.创建socket
        _sock = socket(AF_INET, SOCK_STREAM, 0);
        if (_sock < 0)
        {
            std::cerr << "socket create error" << std::endl;
            exit(2);
        }
        //2.tcp的客户端要不要bind？需要！但不需要显式bind。这个尤其是client，port要让OS确定
        //3.要不要listen？不需要！
        //4.要不要accept？不需要！
        //5.要什么？要发起链接！(connect) 

    }

    void start()
    {
        struct sockaddr_in server;
        memset(&server, 0, sizeof server);
        server.sin_family = AF_INET;
        server.sin_port = htons(_serverport);
        server.sin_addr.s_addr = inet_addr(_serverip.c_str());

        if (connect(_sock, (struct sockaddr*)&server, sizeof server) != 0)
        {
            std::cerr << "socket connect error" << std::endl;
        }
        else
        {
            std::string line;
            std::string inbuffer;
            while (true)
            {
                std::cout << "mycal>>> ";
                std::getline(std::cin, line);
                Request req(10, 10, '+');
               // Request req = ParseLine(line);
                std::string content;
                req.serialize(&content);
                std::string send_string = enLength(content);
                send(_sock, send_string.c_str(), send_string.size(), 0);

                std::string package, text;
                if (!recvPackage(_sock, inbuffer, &package)) 
                    continue;
                if(!deLength(package,&text))
                    continue;
                Response resp;
                resp.deserialize(text);
                std::cout << "eixtCode: " << resp._exitcode << std::endl;
                std::cout << "result: " << resp._result << std::endl;
                 
            }
        }
    }
    void ParseLine(const std::string &line)
    {
        //"1+1" "123*456" "12/0"
        
    }
    ~CalClient()
    {
        if (_sock >= 0) close(0);
    }
private:
    int _sock;
    std::string _serverip;
    uint16_t _serverport;
};

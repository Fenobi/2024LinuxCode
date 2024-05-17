#include <iostream>
#include <string>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <strings.h>
#include <cstdlib>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

uint16_t serverport = 8080;
//string serverip = "192.168.216.128";
string serverip = "127.0.0.1";

int main()
{
    int csock = socket(AF_INET, SOCK_DGRAM, 0);//IPPROTO_UDP

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());

#define NUM 1024
    char inbuffer[NUM];
    string line;
    while (true)
    {
        //发送逻辑
        cout << "Please Enter# ";
        getline(cin, line);
        int n = sendto(csock, line.c_str(), line.size(), 0, (struct sockaddr*)&server, sizeof server);
        if (n < 0)
        {
            cerr << "sendto error!" << endl;
            break;
        }

        struct sockaddr_in peer;
        socklen_t peerlen = sizeof peer;
        //socklen_t peerlen = sizeof peer;
        //接收逻辑
        inbuffer[0] = 0;//C风格的清空
        n = recvfrom(csock, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr*)&peer, &peerlen);
        if (n >= 0)
        {
            inbuffer[n] = 0;
            cout << "server 返回的消息是# " << inbuffer << endl;
        }
        else
            break;

    }
    
    return 0;
}
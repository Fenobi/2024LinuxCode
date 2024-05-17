#pragma warning(disable:4996)
#include <iostream>
#include <cstring>
#include <string>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

uint16_t serverport = 8080;
string serverip = "192.168.216.128";

int main()
{
    WSAData wsd;
    //启动Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        cout << "WSAStartup Error = " << WSAGetLastError() << endl;
        return 0;
    }
    else
        cout << "WSAStartup Success" << endl;

    SOCKET csock = socket(AF_INET, SOCK_DGRAM, 0);//IPPROTO_UDP
    if (csock == SOCKET_ERROR)
    {
        cout << "socket Error = " << WSAGetLastError() << endl;
        return 1;
    }
    else
        cout << "socket Success" << endl;

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
        int peerlen = sizeof peer;
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
    closesocket(csock);

    WSACleanup();
    return 0;
}
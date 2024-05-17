#include "udpServer.hpp"
#include <memory>
#include <fstream>
#include <unordered_map>
#include <signal.h>

using namespace std;
using namespace Server;

const string dictTxt = "./dict.txt";
unordered_map<string, string> dict;

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}

void handlerMessage(int sockfd, string clientip, uint16_t clientport, string message)
{
    //就可以对message进行特定的业务处理，而不用关系message怎么来的 --- server通信和业务逻辑解耦
    string response_message = message;
    response_message += " [server echo]";

    struct sockaddr_in client;
    bzero(&client, sizeof(client));

    client.sin_family = AF_INET;
    client.sin_port = htons(clientport);
    client.sin_addr.s_addr = inet_addr(clientip.c_str());

    sendto(sockfd, response_message.c_str(), response_message.size(), 
    0, (struct sockaddr*)&client, sizeof client);
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = stoi(argv[1]);
    std::unique_ptr<udpServer> user(new udpServer(handlerMessage, port));

    user->initServer();
    user->start();

    return 0;
}
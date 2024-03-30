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

static bool cutString(const string &target, string *s1, string *s2, const string &sep)
{
    auto pos = target.find(sep);
    if(pos == string::npos) return false;
    *s1 = target.substr(0, pos);//[)
    *s2 = target.substr(pos + sep.size());
    return true;
}

static void initDict()
{
    ifstream in(dictTxt, ios::binary);
    if(!in.is_open())
    {
        cerr<<"open file "<<dictTxt<<" error"<<endl;
        exit(OPEN_ERR);
    }
    string line;
    string key, value;
    while (getline(in, line))
    {
        //cout << line << endl;
        if(cutString(line, &key, &value, ":"))
        {
            dict.insert(make_pair(key, value));
        }
    }

    in.close();

    cout << " load dict success" << endl;
}

void reload(int signo)
{
    (void)signo;
    initDict();
}

static void debugPrint()
{
    for(auto &dt : dict)
    {
        cout << dt.first << " # " << dt.second << endl;
    }
}

// void handlerMessage(int sockfd, string clientip, uint16_t clientport, string message)
// {
//     //就可以对message进行特定的业务处理，而不用关系message怎么来的 --- server通信和业务逻辑解耦
//     string response_message;
//     auto iter = dict.find(message);
//     if (iter == dict.end())
//         response_message = "unknown";
//     else
//         response_message = iter->second;
    
//     //开始返回
//     struct sockaddr_in client;
//     bzero(&client, sizeof(client));

//     client.sin_family = AF_INET;
//     client.sin_port = htons(clientport);
//     client.sin_addr.s_addr = inet_addr(clientip.c_str());

//     sendto(sockfd, response_message.c_str(), response_message.size(), 
//     0, (struct sockaddr*)&client, sizeof client);
// }

void handlerMessage(int sockfd, string clientip, uint16_t clientport, string cmd)
{
    //1.cmd解析，ls -a -l
    //2.如果必要，可能需要fork,exec*

    if (cmd.find("rm") != string::npos || cmd.find("mv") != string::npos || cmd.find("rmdir") != string::npos)
    {
        cerr << clientip << ":" << clientport << "正在做一个非法的操作：" << cmd << endl;
        
    }

    string response;
    FILE *fp = popen(cmd.c_str(), "r");
    if(fp == nullptr)
        response = cmd + " exec failed";
    char line[1024];
    while (fgets(line, sizeof line,fp))
    {
        response += line;
    }
    pclose(fp);

    // 开始返回
    struct sockaddr_in client;
    bzero(&client, sizeof(client));

    client.sin_family = AF_INET;
    client.sin_port = htons(clientport);
    client.sin_addr.s_addr = inet_addr(clientip.c_str());

    sendto(sockfd, response.c_str(), response.size(),
           0, (struct sockaddr *)&client, sizeof client);
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = stoi(argv[1]);
    //string ip = argv[1];

    //signal(2, reload);
    initDict();
    //debugPrint();

    std::unique_ptr<udpServer> usvr(new udpServer(handlerMessage, port));

    usvr->initServer();
    usvr->start();

    return 0;
}
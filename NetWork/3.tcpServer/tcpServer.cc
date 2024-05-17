#include "tcpServer.hpp"
#include "daemon.hpp"
#include <memory>

using namespace server;
using namespace std;

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}

//tcp服务器，启动上和udp server一样
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);

    unique_ptr<TcpServer> tsvr(new TcpServer());

    tsvr->initServer();
    daemonSelf();
    tsvr->start();

    while (true)
    {
        sleep(1);//服务器的核心逻辑
    }
        
    return 0;
}
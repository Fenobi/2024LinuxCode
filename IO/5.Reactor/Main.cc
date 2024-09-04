#include "TcpServer.hpp"
#include "Epoller.hpp"
#include "Log.hpp"
#include "Err.hpp"
#include "Sock.hpp"
#include <memory>

using namespace std;
using namespace tcpserver;

static void usage(std::string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}

std::string transaction(const std::string &request)
{
    return request;
}

int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    unique_ptr<TcpServer> svr(new TcpServer(atoi(argv[1])));
    svr->InitServer();
    svr->Dispatch();
    return 0;
}
#include "pollServer.hpp"
#include "err.hpp"
#include "sock.hpp"
#include <memory>

using namespace std;
using namespace poll_ns;

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
    unique_ptr<PollServer> svr(new PollServer(transaction, atoi(argv[1])));
    svr->initServer();
    svr->statr();
    return 0;
}

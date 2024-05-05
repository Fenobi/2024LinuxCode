#include "udpClient.hpp"
#include <memory>

using namespace std;
using namespace Client;

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[1]);
        exit(USAGE_ERR);
    }
    uint16_t serverport = stoi(argv[2]);
    string serverip = argv[1];
    unique_ptr<udpClient> usvr(new udpClient(serverip, serverport));

    usvr->initClient();
    usvr->run();

    return 0;
}
#include "calClient.hpp"
#include <memory>

using namespace std;

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " serverip serverport\n\n";
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    string serverip = argv[1];
    uint16_t serverport = atoi(argv[2]);
    unique_ptr<TcpClient> tcli(new TcpClient(serverip, serverport));

    tcli->initClient();
    tcli->start();

    return 0;
}
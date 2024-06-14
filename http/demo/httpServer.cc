#include "httpServer.hpp"
#include <memory>

using namespace std;
using namespace server;

void Usage(string proc)
{
    cerr << "Usage:\n\t" << proc << " port\r\n\r\n";
}

bool Get(const httpRequest &req, httpResponse &resp)
{
    cout << "-----------------------------http start--------------------------" << endl;
    cout << req.inbuffer << endl;
    cout << "-----------------------------http start--------------------------" << endl;
    std::string respline = "HTTP1.1 200 OK\r\n";
    std::string respblank = "\r\n";
    std::string body = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>for test</title></head><body><p>付青云的第一个网站shut out to</p></body></html>";

    resp.outbuffer += respline;
    resp.outbuffer += respblank;
    resp.outbuffer += body;

    return true;
}

int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    uint16_t port = atoi(argv[1]);
    unique_ptr<httpServer> httpsvr(new httpServer(Get, port));
    httpsvr->initServer();
    httpsvr->start();
}
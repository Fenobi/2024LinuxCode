#include "calServer.hpp"
#include <memory>

using namespace server;
using namespace std;

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}

//req:里面一定是处理好的一个完整的请求对象
//resq：根据req，进行业务处理，填充resp，不用管理任何读取和写入，序列化和反序列化等细节
bool cal(const Request& req, Response &resp)
{
    resp._exitcode = OK;
    resp._result = OK;

    switch(req._op)
    {
        case '+':
        resp._result = req._x + req._y;
        break;
        case '-':
        resp._result = req._x - req._y;
        break;
        case '*':
        resp._result = req._x * req._y;
        break;
        case '/':
        {
            if(req._y == 0)
                resp._exitcode = DIV_ZERO;
            else
                resp._result = req._x / req._y;
        }
        break;
        case '%':
        {
            if(req._y == 0)
                resp._exitcode = MOD_ZERO;
            else
                resp._result = req._x % req._y;
        }
        break;
        default:
        resp._exitcode = OP_ERROR;
        break;
    }


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

    unique_ptr<CalServer> tsvr(new CalServer());

    tsvr->initServer();
    tsvr->start(cal);

    while (true)
    {
        sleep(1);//服务器的核心逻辑
    }
        
    return 0;
}
#include "TcpServer.hpp"
#include <memory>

using namespace tcpserver;

static void usage(std::string proc)
{
    std::cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}

bool cal(const Request &req, Response &resp)
{
    resp._exitcode = OK;
    resp._result = OK;

    switch (req._op)
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
        if (req._y == 0)
            resp._exitcode = DIV_ZERO;
        else
            resp._result = req._x / req._y;
    }
    break;
    case '%':
    {
        if (req._y == 0)
            resp._exitcode = MOD_ZERO;
        else
            resp._result = req._x % req._y;
    }
    break;
    default:
        resp._exitcode = OP_ERROR;
        break;
    }
    return true;
}

void calculate(Connection *conn)
{
    std::string onePackage;
    while (ParsePackage(conn->inbuffer_, &onePackage))
    {
        std::string reqStr;
        if (!deLength(onePackage, &reqStr))
            return;
        std::cout << "去掉报头的正文：\n"
                  << reqStr << std::endl;

        // 二、对请求Request，反序列化
        // 1、得到一个结构化的请求对象
        Request req;
        if (!req.deserialize(reqStr))
            return;

        // 三、计算处理，req,x req,op req,y
        // 1.得到一个结构化的响应
        Response resp;
        cal(req, resp); // req的处理结果全部放入resp中，回调

        // 四、对响应Response，进行序列化
        // 1.得到一个“字符串”
        std::string respStr;
        resp.serialize(&respStr);

        conn->outbuffer_ += enLength(respStr);
        std::cout << "---------------result: "
                 << conn->outbuffer_ << std::endl;
    }
    if (conn->sender_)
        conn->sender_(conn);

    //     // 如果没有发送完毕，需要对对应的sock开启对写事件的关系，如果发完了，我们要关闭对写事件的关系
    //     if (!conn->outbuffer_.empty())
    //         conn->tsp_->EnableReadWrite(conn, true, true);
    //     else
    //         conn->tsp_->EnableReadWrite(conn, true, false);
}

std::string transaction(const std::string &request)
{
    return request;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    std::unique_ptr<TcpServer> svr(new TcpServer(calculate, atoi(argv[1])));
    svr->InitServer();
    svr->Dispatcher();
    return 0;
}
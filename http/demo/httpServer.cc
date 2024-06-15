#include "httpServer.hpp"
#include <memory>

using namespace std;
using namespace server;

void Usage(string proc)
{
    cerr << "Usage:\n\t" << proc << " port\r\n\r\n";
}

std::string suffixToDesc(const std::string suffix)
{
    std::string ct = "Content-Type: ";
    if (suffix == ".html")
        ct += "text/html";
    else if (suffix == ".jpg")
        ct += "image/jpeg";
    else if (suffix == ".png")
        ct += "image/png";
    ct += "\r\n";
    return ct;
}

// 1、服务器和网页分离，html
// 2、url -> / : web根目录
// 3、我们要正确的给客户端返回资源类型：资源后缀
bool Get(const httpRequest &req, httpResponse &resp)
{
    if (req.path == "test.py")
    {
        // 建立进程间通信，pipe
        // fork创建子进程，execl("/bin/python",test.py);
        // req.parm
        // 使用自己写的C++方法，提供服务
    }
    if (req.path == "/search")
    {
        // req.parm
        // 使用自己写的C++方法，提供服务    
    }

    cout << "-----------------------------http start--------------------------" << endl;
    cout << req.inbuffer << endl;
    std::cout << "method: " << req.method << std::endl;
    std::cout << "url: " << req.url << std::endl;
    std::cout << "httpverion: " << req.httpverion << std::endl;
    std::cout << "path: " << req.path << std::endl;
    std::cout << "suffix: " << req.suffix << std::endl;
    // std::cout << "size: " << req.size << "字节" << std::endl;
    cout << "-----------------------------http end--------------------------" << endl;

    std::string respline = "HTTP1.1 200 OK\r\n";
    std::string respheader = suffixToDesc(req.suffix);
    // if (req.size > 0)
    // {
    //     respheader += "Content-Length: ";
    //     respheader += std::to_string(req.size);
    //     respheader += "\r\n";
    // }
    std::string respblank = "\r\n";
    // std::string body = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>for test</title></head><body><p>付青云的第一个网站</p></body></html>";

    std::string body;
    // body.resize(req.size + 1);
    if (!Util::readFile(req.path, &body))
    {
        Util::readFile(html_404, &body);
    }
    resp.outbuffer += respline;
    resp.outbuffer += respheader;
    resp.outbuffer += respblank;
    resp.outbuffer += body;

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    uint16_t port = atoi(argv[1]);
    unique_ptr<httpServer> httpsvr(new httpServer(Get, port));

    httpsvr->registerCb("/", Get);//功能路由
    // httpsvr->registerCb("/search", Search);
    // httpsvr->registerCb("/test.py", Other);
    httpsvr->initServer();
    httpsvr->start();
}
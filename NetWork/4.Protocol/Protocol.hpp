#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include <jsoncpp/json/json.h>

#define SEP " "
#define SEP_LEN strlen(SEP)
#define LINE_SEP "\r\n"
#define LINE_SEP_LEN strlen(LINE_SEP)

enum RESULT
{
    OK = 0,
    DIV_ZERO,
    MOD_ZERO,
    OP_ERROR
};

std::string enLength(const std::string &text)
{
    std::string send_string = std::to_string(text.size());
    send_string += LINE_SEP;
    send_string += text;
    send_string += LINE_SEP;

    return send_string;
}

bool deLength(const std::string &package,std::string *text)
{
    auto pos = package.find(LINE_SEP);
    if(pos == std::string::npos)
        return false;
    std::string text_len_string = package.substr(0, pos);
    int text_len = std::stoi(text_len_string);
    *text = package.substr(pos + LINE_SEP_LEN, text_len);
    return true;
}

//如何让系统知道我们用的是哪个协议呢？
//"content_len"\r\n"协议编号"\r\n"x op y"\r\n


class Request
{
public:
    Request() : _x(0), _y(0), _op(0)
    {}
    Request(int x,int y,char op) :_x(x), _y(y), _op(op)
    {}
    bool serialize(std::string *out)
    {
#ifdef MYSELF
        *out = " ";
        // 结构化 ->"x op y"
        std::string x_string = std::to_string(_x);
        std::string y_string = std::to_string(_y);

        *out = x_string;
        *out += SEP;
        *out += _op;
        *out += SEP;
        *out += y_string;
#else//用现成的
        Json::Value root;
        root["first"] = _x;
        root["second"] = _y;
        root["oper"] = _op;

        Json::FastWriter writer;
        // Json::StyledWriter writer;
        *out = writer.write(root);
#endif
        return true;
    }
    bool deserialize(const std::string &in)
    {
#ifdef MYSELF       
        auto left = in.find(SEP);
        auto right = in.rfind(SEP);

        if(left == std::string::npos || right==std::string::npos) return false;
        if(right==left) return false;
        if (right - (left + SEP_LEN) !=1) return false;
        std::string x_string = in.substr(0, left); //[0,2) abcd->ab start,start-end
        std::string y_string = in.substr(right + SEP_LEN);

        if(x_string.empty())
            return false;
        if (y_string.empty())
            return false;
        _x = std::stoi(x_string);
        _y = std::stoi(y_string);
        _op = in[left + SEP_LEN];
#else
        Json::Value root;
        Json::Reader reader;
        reader.parse(in, root);

        _x = root["first"].asInt();
        _y = root["second"].asInt();
        _op = root["oper"].asInt();
#endif
        return true;
    }

public:
    //"x op y"
    int _x;
    int _y;
    char _op;
};


class Response
{
public:
    Response() :_exitcode(0), _result(0)
    {}
    Response(int exitcode, int result) : _exitcode(exitcode), _result(result)
    {
    }
    bool serialize(std::string *out)
    {
#ifdef MYSELF
        *out = "";
        std::string ec_string = std::to_string(_exitcode);
        std::string res_string = std::to_string(_result);

        *out = ec_string;
        *out += SEP;
        *out += res_string;
#else
        Json::Value root;
        root["exitcode"] = _exitcode;
        root["result"] = _result;

        Json::FastWriter writer;
        *out = writer.write(root);
#endif
        return true;
    }

    bool deserialize(const std::string &in)
    {
#ifdef MYSELF
        auto mid = in.find(SEP);
        if(mid == std::string::npos)
            return false;
        std::string ec_string = in.substr(0, mid);
        std::string res_string = in.substr(mid+SEP_LEN);

        if(ec_string.empty() || res_string.empty())
            return false;

        _exitcode = std::stoi(ec_string);
        _result = std::stoi(res_string);
#else
        Json::Value root;
        Json::Reader reader;
        reader.parse(in, root);

        _exitcode = root["exitcode"].asInt();
        _result = root["result"].asInt();
#endif
        return true;
    }

public:
    int _exitcode;//0:计算成功 ！0表示计算失败，具体是多少，定好标准
    int _result;  //计算结果
};

//"content_len"\r\n "x op y"\r\n 
bool recvPackage(int sock, std::string &inbuffer, std::string *text)
{
    char buffer[1024];
    while (true)
    {
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if(n > 0)
        {
            buffer[n] = 0;
            inbuffer += buffer;//追加
            // 分析处理
            auto pos = inbuffer.find(LINE_SEP);
            if(pos == std::string::npos)
                continue;
            std::string text_len_string = inbuffer.substr(0, pos);
            int text_len = std::stoi(text_len_string);
            int total_len = text_len_string.size() + 2 * LINE_SEP_LEN + text_len;
            if (inbuffer.size() < total_len )
                continue;

            std::cout << "处理前#inbuffer: \n" << inbuffer << std::endl;

            //至少有一个完整的报文
            *text = inbuffer.substr(0, total_len);
            inbuffer.erase(0, total_len);

            std::cout << "处理后#inbuffer: \n" << inbuffer << std::endl;

            break;
        }
        else
            return false;
    }
    return true;
}

bool recvRequestAll(int sock, std::string &inbuffer, std::vector<std::string> *out)
{
    std::string line;
    while (recvPackage(sock, inbuffer, &line))
    {
        out->push_back(line);
    }
}
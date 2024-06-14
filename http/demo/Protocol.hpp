#pragma once

#include <iostream>
#include <string>
#include <vector>

class httpRequest
{
public:
    std::string inbuffer;
    // std::string reqline;
    // std::vector<std::string> reqheader;
    // std::string body;

    // std::string method;
    // std::string url;
    // std::string httpverion;
};

class httpResponse
{
public:
    std::string outbuffer;
};
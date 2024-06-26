#pragma once

#include <iostream>
#include <string>
#include <fstream>

class Util
{
public:
    static std::string getOneLine(std::string &buffer, const std::string &sep)
    {
        auto pos = buffer.find(sep);
        if(pos == std::string::npos)
            return "";
        std::string sub = buffer.substr(0, pos);
        //buffer.erase(0, sub.size() + sep.size());
        return sub;
    }
    static bool readFile(const std::string resource,char *buffer,int size)
    {
        std::ifstream in(resource, std::ios::binary);
        if (!in.is_open())
        {
            std::cout << "open: " << resource << " fail!" << std::endl;
            return false; // resource not found
        }
        in.read(buffer, size);
        // std::string line;//图片、视频不能按行读
        // while (std::getline(in, line))
        // {
        //     *out += line;
        // }

        in.close();
        return true;
    }
};
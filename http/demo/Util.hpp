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
    static bool readFile(const std::string resource,std::string *out)
    {
        std::ifstream in(resource);
        if(!in.is_open())
        {
            std::cout << "open: " << resource << " fail!" << std::endl;
            return false; // resource not found
        }
        std::string line;
        while (std::getline(in, line))
        {
            *out += line;
        }

        in.close();
        return true;
    }
};
#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

void printLog()
{
    std::cout << "this is a log" << std::endl;
}

void download()
{
    std::cout << "this is a download" << std::endl;
}

void executeSql()
{
    std::cout << "this is a executeSql" << std::endl;
}

void setNoBlock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0)
    {
        std::cerr << "fcnlt:" << strerror(errno) << std::endl;
        return;
    }
    fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}
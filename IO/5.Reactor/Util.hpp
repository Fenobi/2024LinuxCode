#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#include "Log.hpp"

class Util
{
public:
    static bool SetNonBlock(int fd)
    {
        int fl = fcntl(fd, F_GETFD);
        if(fl<0)
        {
            logMessage(WARNING, "fcntl err code: %d errstring: %s", errno, strerror(errno));
            return false;
        }
        fcntl(fd, F_SETFL, fl | O_NONBLOCK);
        return true;
    }
};
#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>

class Util
{
public:
    static bool SetNonBlock(int fd)
    {
        int fl = fcntl(fd, F_GETFL);
        if (fl < 0) return false;
        fcntl(fd, F_SETFL, fl | O_NONBLOCK);
        return true;
    }
};
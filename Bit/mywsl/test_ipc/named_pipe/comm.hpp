#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <cassert>
#include <fcntl.h>

#define NAMED_PIPE "/tmp/mypipe.ddy"

bool createFifo(const std::string &path)
{
    umask(0);
    int n = mkfifo(path.c_str(), 0600);
    if (n == 0)
        return true;
    else
    {
        std::cout << "errno: " << errno << " err string: " << strerror(errno) << std::endl;
        return false;
    }
}

bool removeFifo(const std::string &path)
{
    int n = unlink(path.c_str());
    assert(n == 0);
    (void)n;
    return true;
}
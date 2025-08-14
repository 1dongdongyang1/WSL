#include "util.hpp"
#include <functional>
#include <vector>
#include <sys/select.h>

using func_t = std::function<void()>;

#define INIT(v) do{\
            v.push_back(printLog);\
            v.push_back(download);\
            v.push_back(executeSql);\
        }while(0)

#define EXECTE_OTHER(cbs) do{\
            for(auto const& cb : cbs) cb();\
        }while(0)


int main()
{
    fd_set;
    std::vector<func_t> cbs;
    INIT(cbs);

    setNonBlock(0);
    char buffer[1024];
    while (true)
    {
        printf(">>> ");
        fflush(stdout);
        ssize_t s = read(0, buffer, sizeof(buffer) - 1);
        if (s > 0)
        {
            buffer[s-1] = 0;
            std::cout << "echo# " << buffer << std::endl;
        }
        else if (s == 0)
        {
            std::cout << "read end" << std::endl;
            break;
        }
        else
        {
            if (errno == EAGAIN) EXECTE_OTHER(cbs);
            else if (errno == EINTR) continue;
            else {
                std::cout << "s: " << s << " " << strerror(errno) << std::endl;
            }
        }
        sleep(1);
    }
}
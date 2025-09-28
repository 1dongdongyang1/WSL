#include "epollServer.hpp"
#include "err.hpp"
#include <memory>

using namespace std;
using namespace epoll_ns;

static void Usage(std::string proc)
{
    cerr << "Usage:\n\t" << proc << " port\n" << endl;
}

string transaction(const string& request)
{
    return request;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    unique_ptr<EpollServer> epollsvr(new EpollServer(transaction, atoi(argv[1])));

    epollsvr->initServer();
    epollsvr->start();

    return 0;
}
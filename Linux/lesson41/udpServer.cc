#include "udpServer.hpp"
#include <memory>

using namespace Server;

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}

void handlerMessage(string clientIp, uint16_t clientPort, string message)
{
    // server通信与业务解耦
}

// ./udpServer server_port
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ARR);
    }
    uint16_t port = atoi(argv[1]);

    unique_ptr<udpServer> usvr(new udpServer(handlerMessage, port));

    usvr->initServer();
    usvr->start();
    return 0;
}
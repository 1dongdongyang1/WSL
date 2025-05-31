#include "calClient.hpp"
#include <memory>

using namespace client;

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}
// ./tcpClient serverip serverport
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    string serverip = argv[1];
    uint16_t serverport = atoi(argv[2]);

    unique_ptr<CalClient> tcli(new CalClient(serverip, serverport));
    tcli->initClient();
    tcli->start();

    return 0;
}
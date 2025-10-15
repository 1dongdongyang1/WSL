#include "ChatServer.h"

int main() {
    ChatServer server(8080);
    server.setThreadNum(4);
    server.start();
    return 0;
}
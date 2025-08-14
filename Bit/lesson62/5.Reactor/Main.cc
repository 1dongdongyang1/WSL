#include "TcpServer.hpp"
#include <memory>

using namespace tcpserver;

static void Usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << " port\n" << std::endl;
}

bool cal(const Request& req, Response& resp)
{
    // req已经有结构化完成的数据了，可以直接计算
    switch (req.op)
    {
    case '+':
        resp.result = req.x + req.y;
        break;
    case '-':
        resp.result = req.x - req.y;
        break;
    case '*':
        resp.result = req.x * req.y;
        break;
    case '/':
    {
        if (req.y == 0) resp.exitcode = DIV_ZERO;
        else resp.result = req.x / req.y;
    }
    break;
    case '%':
    {
        if (req.y == 0) resp.exitcode = MOD_ZERO;
        else resp.result = req.x % req.y;
    }
    break;
    default:
        resp.exitcode = OP_ERROR;
        break;
    }

    return true;
}

void calculate(Connection* conn)
{
    std::string onePackage;
    while (ParseOnePackage(conn->_inbuffer, &onePackage))
    {
        std::string reqStr;
        if (!deLength(onePackage, &reqStr)) return;

        Request req;
        if (!req.deserialize(reqStr)) return;

        Response resp;
        cal(req, resp);

        std::string respStr;
        resp.serialize(&respStr);

        conn->_outbuffer += enLength(respStr);
        std::cout << conn->_outbuffer << std::endl;
    }
    // 发回去
    if (conn->_sender) conn->_sender(conn);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    std::unique_ptr<TcpServer> tsvr(new TcpServer(calculate, port));
    tsvr->InitServer();
    tsvr->Dispatcher();
    return 0;
}
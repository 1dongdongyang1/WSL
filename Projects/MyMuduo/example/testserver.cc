#include <mymuduo/TcpServer.h>
#include <mymuduo/Logger.h>

#include <string>
#include <functional>

class EchoServer
{
public:
    EchoServer(EventLoop* loop,
        const InetAddress& listenAddr,
        const std::string& name)
        : server_(loop, listenAddr, name)
        , loop_(loop)
    {
        server_.setConnectionCallback(std::bind(&EchoServer::OnConnection, this, std::placeholders::_1));
        server_.setMessageCallback(std::bind(&EchoServer::OnMessage,
            this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        server_.setThreadNum(3);
    }
    void start()
    {
        server_.start();
    }
private:
    void OnConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected()) LOG_INFO("connected\n");
        else
        {
            conn->shutdown();
            LOG_INFO("disconnected\n");
        }
    }

    void OnMessage(const TcpConnectionPtr& conn,
        Buffer* buf,
        Timestamp stamp)
    {
        std::string msg = buf->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown();
    }

    EventLoop* loop_;
    TcpServer server_;
};

int main()
{
    InetAddress listenAddr;
    EventLoop loop;
    EchoServer server(&loop, listenAddr, "EchoServer");
    server.start();
    loop.loop();
}
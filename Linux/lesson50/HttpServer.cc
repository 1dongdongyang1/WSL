#include "HttpServer.hpp"
#include <memory>

using namespace server;

void Usage(string proc)
{
    cerr << "\nUsage:\n\t" << proc << " port\n\n";
}

std::string suffixToDesc(const std::string suffix)
{
    std::string ct = "Content-Type: ";
    if (suffix == ".html") ct += "text/html";
    else if (suffix == ".png") ct += "image/png";

    ct += "\r\n";
    return ct;
}

void Get(const HttpRequest& req, HttpResponse& resp)
{
    cout << "------------------http start--------------" << endl;
    cout << req.inbuffer;
    std::cout << "method: " << req.method << std::endl;
    std::cout << "url: " << req.url << std::endl;
    std::cout << "httpversion: " << req.httpversion << std::endl;
    std::cout << "path: " << req.path << std::endl;
    std::cout << "suffix: " << req.suffix << std::endl;
    std::cout << "size: " << req.size << "字节" << std::endl;
    cout << "------------------http end----------------" << endl;

    string respline = "HTTP/1.1 200 OK\r\n";
    // string respline = "HTTP/1.1 307 TR\r\n";
    string respheader = suffixToDesc(req.suffix);
    if (req.size > 0)
    {
        respheader += "Content-Length: ";
        respheader += std::to_string(req.size);
        respheader += "\r\n";
    }

    respheader += "Set-Cookie: 12315\r\n";

    // respheader += "Location: https://www.qq.com\r\n";
    string respblank = "\r\n";
    // string body = "<html lang=\"en\"><head><meta charset=\"UTF-8\"><title>for test</title> \
    <h1>hello world</h1></head><body><p>东东羊</p></body></html>";
    string body;
    if (!Util::readFile(req.path, &body)) Util::readFile(html_404, &body);

    resp.outbuffer += respline;
    resp.outbuffer += respheader;
    resp.outbuffer += respblank;
    resp.outbuffer += body;
}

// ./httpServer 8080
int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    uint16_t port = atoi(argv[1]);
    unique_ptr<HttpServer> httpsvr(new HttpServer(Get, port));
    httpsvr->initServer();
    httpsvr->start();

    return 0;
}
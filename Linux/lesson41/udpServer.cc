#include "udpServer.hpp"
#include "onlineUser.hpp"
#include <memory>
#include <unordered_map>
#include <fstream>
#include <signal.h>

using namespace Server;

const string dictTxt = "./dict.txt";
unordered_map<string, string> dict;
OnlineUser onlineuser;

static bool cutString(const string& target, string* s1, string* s2, const string& sep)
{
    auto pos = target.find(sep);
    if (pos == string::npos) return false;
    *s1 = target.substr(0, pos);
    *s2 = target.substr(pos + sep.size());
    return true;
}

static void initDict()
{
    ifstream in(dictTxt, ios::binary);
    if (!in.is_open())
    {
        cerr << "open file " << dictTxt << " err " << endl;
        exit(OPEN_ERR);
    }

    string line, key, val;
    while (getline(in, line))
    {
        if (cutString(line, &key, &val, ":"))    //&输出型参数
        {
            dict.insert(make_pair(key, val));
        }
    }

    in.close();
}

static void reload(int signo)
{
    (void)signo;
    initDict();
    cout << "load dict success" << endl;
}

static void debugPrint()
{
    for (auto& dt : dict)
    {
        cout << dt.first << "#" << dt.second << endl;
    }
}

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}

// dome1
void handlerMessage(int sockfd, string clientIp, uint16_t clientPort, string message)
{
    // server通信与业务解耦
    // 翻译
    string response_message;
    auto iter = dict.find(message);
    if (iter == dict.end()) response_message = "unknown";
    else response_message = iter->second;

    // 发回去
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(clientIp.c_str());
    client.sin_port = htons(clientPort);

    sendto(sockfd, response_message.c_str(), response_message.size(), 0, (struct sockaddr*)&client, sizeof(client));
}

// dome2
void execConmmand(int sockfd, string clientIp, uint16_t clientPort, string cmd)
{
    // 1.cmd解析, ls -a -l
    // 2.如需要，会调用 fork, pipe, exec*
    if (cmd.find("rm") != string::npos || cmd.find("mv") != string::npos || cmd.find("rmdir") != string::npos)
    {
        cerr << clientIp << ":" << clientPort << "非法操作:" << cmd << endl;
        return;
    }

    string response;
    FILE* fp = popen(cmd.c_str(), "r");
    if (fp == nullptr) response = cmd + " exec failed";
    char line[1024];
    while (fgets(line, sizeof(line), fp)) response += line;
    pclose(fp);

    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(clientIp.c_str());
    client.sin_port = htons(clientPort);

    sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr*)&client, sizeof(client));
}

//demo3
void routeMessage(int sockfd, string clientIp, uint16_t clientPort, string message)
{
    if (message == "online") onlineuser.addUser(clientIp, clientPort);
    if (message == "offline") onlineuser.delUser(clientIp, clientPort);
    if (onlineuser.isOnline(clientIp, clientPort))
    {
        // 消息的路由
        onlineuser.broadcastMessage(sockfd, clientIp, clientPort, message);
    }
    else
    {
        struct sockaddr_in client;
        memset(&client, 0, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_addr.s_addr = inet_addr(clientIp.c_str());
        client.sin_port = htons(clientPort);

        string response = "当前没有上线, 运行: online 后上线";
        sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr*)&client, sizeof(client));
    }
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

    initDict();
    debugPrint();
    signal(2, reload);  //热加载

    unique_ptr<udpServer> usvr(new udpServer(handlerMessage, port));
    // unique_ptr<udpServer> usvr(new udpServer(execConmmand, port));
    // unique_ptr<udpServer> usvr(new udpServer(routeMessage, port));

    usvr->initServer();
    usvr->start();
    return 0;
}
#include "friend.pb.h"
#include "mprpcapplication.h"
#include <string>
#include <vector>
#include <iostream>

class FriendService : public fixbug::FriendServiceRPC
{
public:
    std::vector<std::string> GetFriendList(uint32_t userid)
    {
        std::cout << "GetFriendList()" << std::endl;
        std::vector<std::string> vec;
        vec.push_back("ddy");
        vec.push_back("ym");
        return vec;
    }

    void GetFriendList(::google::protobuf::RpcController* controller,
        const ::fixbug::GetFriendListRequest* request,
        ::fixbug::GetFriendListResponse* response,
        ::google::protobuf::Closure* done)override
    {
        uint32_t userid = request->userid();
        std::vector<std::string> FriendList = GetFriendList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for(std::string& name : FriendList)
        {
            std::string* p = response->add_friends();
            *p = name;
        }
        done->Run();
    }
};

int main(int argc, char** argv)
{
    // 调用框架的初始化
    MPRPCApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象，把FriendService对象发布到rpc节点上
    RPCProvider provider;
    provider.NotifyService(new FriendService());

    // 启动rpc服务节点，阻塞等待远程rpc调用请求
    provider.Run();

    return 0;
}
#include "friend.pb.h"
#include "mprpcapplication.h"
#include <iostream>

int main(int argc, char** argv)
{
    // 整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数(只初始化一次)
    MPRPCApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::FriendServiceRPC_Stub stub(new MPRPCChannel());
    // rpc方法的请求参数
    fixbug::GetFriendListRequest request;
    request.set_userid(1);
    // rpc方法的响应
    fixbug::GetFriendListResponse response;
    // 发起rpc方法的调用 同步的rpc调用过程 MPRPCChannel::callmethod
    MPRPCController controller;
    stub.GetFriendList(&controller, &request, &response, nullptr);

    // 一次rpc调用完成，读调用的结果
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        if (0 == response.result().errcode())
        {
            std::cout << "getfriendlist response:" << std::endl;
            for (int i = 0; i < response.friends_size(); i++)
                std::cout << "index:" << (i + 1) << " name:" << response.friends(i) << std::endl;
        }
        else
        {
            std::cout << "getfriendlist response error:" << response.result().errmsg() << std::endl;
        }
    }

    return 0;
}
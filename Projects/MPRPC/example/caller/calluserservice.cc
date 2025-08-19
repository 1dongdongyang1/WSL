#include "user.pb.h"
#include "mprpcchannel.h"
#include "mprpcapplication.h"
#include <iostream>

int main(int argc, char** argv)
{
    // 整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数(只初始化一次)
    MPRPCApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRPC_Stub stub(new MPRPCChannel());
    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("ddy");
    request.set_pwd("123");
    // rpc方法的响应
    fixbug::LoginResponse response;
    // 发起rpc方法的调用 同步的rpc调用过程 MPRPCChannel::callmethod
    stub.Login(nullptr, &request, &response, nullptr);

    // 一次rpc调用完成，读调用的结果
    if(0 == response.result().errcode())    
    {
        std::cout << "rpc login response:" << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error:" << response.result().errmsg() << std::endl;
    }

    fixbug::RegisterRequest req;
    fixbug::RegisterResponse rsp;
    req.set_id(1);
    req.set_name("ym");
    req.set_pwd("111");
    stub.Register(nullptr, &req, &rsp, nullptr);
    if(0 == rsp.result().errcode())    
    {
        std::cout << "rpc resgister response:" << rsp.success() << std::endl;
    }
    else
    {
        std::cout << "rpc register response error:" << rsp.result().errmsg() << std::endl;
    }

    return 0;
}
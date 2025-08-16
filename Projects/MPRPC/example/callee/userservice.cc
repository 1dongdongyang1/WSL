#include "user.pb.h"
#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <iostream>
#include <string>

/*
UserService本来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendList
-> 本地的另外的进程，想调用当前进程的本地方法 -> 进程间通信 -> 管道/消息队列/共享内存
-> 另外一台主机上的进程，想调用当前本机进程的方法 -> 网络通信
*/

class UserService : public fixbug::UserServiceRPC   // 使用在rpc服务发布端
{
public:
    bool Login(const std::string& name, const std::string& pwd)
    {
        std::cout << "Login()" << std::endl;
        return true;
    }
    
    // 重写基类UserServiceRPC的虚函数，下面的方法是框架来直调用的
    virtual void Login(::google::protobuf::RpcController* controller, 
        const ::fixbug::LoginRequest* request,
        ::fixbug::LoginResponse* response,
        ::google::protobuf::Closure* done)
    {
        // 框架上报给业务，做业务处理
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 做本地业务
        bool login_result = Login(name, pwd);

        // 响应写入
        fixbug::ResultCode* rc = response->mutable_result();
        rc->set_errcode(0);
        rc->set_errmsg("");
        response->set_success(login_result);

        // 执行回调     -> 进行响应的序列化和网络发送(框架实现)
        done->Run();
    }
};

int main(int argc, char** argv)
{
    // 调用框架的初始化
    MPRPCApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象，把UserService对象发布到rpc节点上
    RPCProvider provider;
    provider.NotifyService(new UserService());

    // 启动rpc服务节点，阻塞等待远程rpc调用请求
    provider.Run();

    return 0;
}
#include "test.pb.h"
#include <string>
#include <iostream>
using namespace fixbug;

void test1()
{
    LoginRequest req;
    req.set_name("ddy");
    req.set_pwd("123456");

    std::string send_str;
    if (req.SerializeToString(&send_str))
    {
        std::cout << send_str.c_str() << std::endl;
    }

    LoginRequest reqB;
    if (reqB.ParseFromString(send_str))
    {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }
}

void test2()
{
    GetFriendListResponse rsp;
    ResultCode* rc = rsp.mutable_result();
    rc->set_errcode(0);

    User* user1 = rsp.add_friend_list();
    user1->set_name("ddy");
    user1->set_age(10);
    user1->set_sex(User::MAN);

    User* user2 = rsp.add_friend_list();
    user2->set_name("ym");
    user2->set_age(9);
    user2->set_sex(User::WOMAN);

    std::cout << rsp.friend_list_size() << std::endl;

    std::string send_str;
    if(rsp.SerializeToString(&send_str))
    {
        std::cout << send_str << std::endl;
    }

    GetFriendListResponse rspB;
    if(rspB.ParseFromString(send_str))
    {
        std::cout << rspB.result().errcode() << std::endl;
        std::cout << rspB.result().errmsg() << std::endl;
        for (int i = 0; i < rspB.friend_list_size(); i++)
        {
            std::cout << rspB.friend_list()[i].name() << std::endl;
            std::cout << rspB.friend_list()[i].age() << std::endl;
            std::cout << rspB.friend_list()[i].sex() << std::endl;
            std::cout << std::endl;
        }
    }

}

int main()
{
    // test1();
    test2();

    return 0;
}
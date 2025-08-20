#pragma once

#include "rpcprovider.h"
#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

// MPRPC框架的基础类，负责框架的一些初始化
class MPRPCApplication
{
public:
    static void Init(int argc, char** argv);
    static MPRPCApplication& GetInstance();
    static MPRPCConfig& GetConfig();
private:
    MPRPCApplication() {}
    MPRPCApplication(const MPRPCApplication&) = delete;
    MPRPCApplication(MPRPCApplication&&) = delete;
private:
    static MPRPCConfig m_config;
};
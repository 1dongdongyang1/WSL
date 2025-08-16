#pragma once

#include "mprpcconfig.h"

// MPRPC框架的基础类，负责框架的一些初始化
class MPRPCApplication
{
public:
    static void Init(int argc, char** argv);
    static MPRPCApplication& GetInstance();
private:
    MPRPCApplication() {}
    MPRPCApplication(const MPRPCApplication&) = delete;
    MPRPCApplication(MPRPCApplication&&) = delete;
private:
    static MPRPCConfig m_config;
};
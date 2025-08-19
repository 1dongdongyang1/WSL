#include "mprpccontroller.h"

MPRPCController::MPRPCController()
{
    m_failed = false;
    m_errText = "";
}

void MPRPCController::Reset()
{
    m_failed = false;
    m_errText = "";
}

bool MPRPCController::Failed()const
{
    return m_failed;
}

std::string MPRPCController::ErrorText()const
{
    return m_errText;
}

void MPRPCController::SetFailed(const std::string& reason)
{
    m_failed = true;
    m_errText = reason;
}

// 目前未实现具体的功能
void MPRPCController::StartCancel() {}
bool MPRPCController::IsCanceled()const { return false; }
void MPRPCController::NotifyOnCancel(google::protobuf::Closure* callback) {}
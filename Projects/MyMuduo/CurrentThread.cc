#include "CurrentThread.h"

namespace CurrentThread
{
    __thread int t_cachedTid = 0;

    void cacheTid()
    {
        // 通过Linux系统调用，获取当前线程的tid值
        if (t_cachedTid == 0) t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
    }
}
/*
开发日志
1. 接口设计
    ThreadPool tp;      -> 可设计成单例模式
    tp.setMode(fixed(default)/cached);
    tp.run();
    Result result = tp.submitTask(Task&);   -> 异步提交处理任务
2. ThreadPool类设计
    1. 管理线程的数据结构 -> 数组 -> fixed/cached
    2. 线程的数量   -> CPU密集型 和 IO密集型
                   -> CPU密集型 -> 线程数量 = CPU核数
                   -> IO密集型  -> 线程数量 = 2*CPU核数 + 2
    3. 管理任务队列的数据结构 -> ringqueue

*/

#pragma once

#include "ringqueue.hpp"
#include <thread>
#include <vector>

const static int g_num = 16;

class Task  // 任务抽象基类
{
public:
    virtual void run() = 0;
};

enum class PoolMode
{
    MODE_FIXED,
    MODE_CACHED
};

class ThreadPool
{
public:
    ThreadPool(const int num = g_num);

    void setMode(PoolMode);
    void run();


private:
    std::vector<std::thread*> _threads;
    int _threadNum;
    RingQueue<Task*> _taskQueue;
};

/*
开发日志
1. 接口设计
    ThreadPool tp;      -> 可设计成单例模式
    tp.run();
    Result result = tp.submitTask(Task&);   -> 异步提交处理任务
2. 数据设计
    1. 管理线程的数据结构 -> 数组
    2. 管理任务队列的数据结构 -> ringqueue
3. 细节
    1. 线程的数量   -> CPU密集型 和 IO密集型
                   -> CPU密集型 -> 线程数量 = CPU核数
                   -> IO密集型  -> 线程数量 = 2*CPU核数 + 2
    2. 
*/

#pragma once

#include "ringqueue.hpp"
#include <thread>
#include <vector>
#include <memory>
#include <functional>
#include <condition_variable>

const static int g_threadnum = 16;

class Task  // 任务抽象基类
{
public:
	virtual void run() = 0;
	virtual ~Task() = default;
};

class ThreadPool
{
public:
	ThreadPool(const int num = g_threadnum);
	~ThreadPool();
	void submit(std::shared_ptr<Task>);

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
private:
	std::vector<std::thread> _threads;
	RingQueue<std::shared_ptr<Task>> _taskQueue;
	std::atomic<bool> _stop;

	void workerLoop();
};

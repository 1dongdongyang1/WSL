/*
开发日志
1. 接口设计
    ThreadPool tp;      -> 可设计成单例模式
    Result result = tp.submit(Task&);   -> 异步提交处理任务
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

const static int g_threadnum = 16;

class Any	// 可以接收任意类型的数据
{
public:
	Any() = default;
	~Any() = default;
	Any(const Any&) = delete;
	Any& operator=(const Any&) = delete;
	Any(Any&&) = default;
	Any& operator=(Any&&) = default;

	// 该构造可以让Any类型接收任意类型的数据
	template<class T>
	Any(T data):_base(std::make_unique<Derive<T>>(data))
	{}

	// 该方法可以将Any对象里面存储的data数据提取出来
	template<class T>
	T cast()
	{
		Derive<T>* pd = dynamic_cast<Derive<T>*>(_base.get());
		if (pd == nullptr) throw "type is unmatch";
		return pd->_data;
	}

private:
	class Base
	{
	public:
		virtual ~Base() = default;
	};

	template<class T>
	class Derive : public Base
	{
	public:
		Derive(T data):_data(data)
		{}

		T _data;
	};
private:
	std::unique_ptr<Base> _base;
};

class Task  // 任务抽象基类
{
public:
	virtual Any run() = 0;
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
	void workerLoop();
private:
	std::vector<std::thread> _threads;
	RingQueue<std::shared_ptr<Task>> _taskQueue;
	std::atomic<bool> _stop;
};

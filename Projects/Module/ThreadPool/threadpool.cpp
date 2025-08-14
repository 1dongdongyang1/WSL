#include "threadpool.hpp"

ThreadPool::ThreadPool(const int num) :_threads(), _taskQueue(), _stop(false)
{
	_threads.reserve(num);
	for (int i = 0; i < num; i++)
	{
		_threads.emplace_back(&ThreadPool::workerLoop, this);
	}
}

ThreadPool::~ThreadPool()
{
	_stop = true;

	for (size_t i = 0; i < _threads.size(); ++i) _taskQueue.push(nullptr);
	for (auto& t : _threads) 
	{
		if (t.joinable()) t.join();
	}
}

void ThreadPool::submit(std::shared_ptr<Task> task)
{
	_taskQueue.push(task);
}

void ThreadPool::workerLoop()
{
	while (true)
	{
		std::shared_ptr<Task> task;
		_taskQueue.pop(&task);
		if (_stop && !task) break; 
		if (task) task->run();
	}
}

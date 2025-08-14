/*
开发日志
1. 接口设计
   环形队列是给生产者-消费者模型和线程池提供缓冲区，本质上还是作为队列，提供给上层接口 
   需要考虑的是多线程并发问题  -> 上层不再关心线程安全问题
2. 底层数据结构 -> 数组
3. 同步机制
   同一时间只能一个生产者能放入     -> 一个锁
   同一时间只能一个消费者能取出     -> 一个锁
   同一时间放入和取出可以同时进行，但是不能同时修改同一个位置      
   -> 我不管你是放入还是取出，我只给一把锁，谁拿到谁修改
4. 细节
    1. 环形队列是模板类，声明定义不能拆开文件写
    2. -std=c++20
    3. 对比队列，环形队列最大的问题是无法扩容，好处是线程安全，且不用频繁创建回收内存
*/

#pragma once

#include <semaphore>
#include <vector>
#include <mutex>

const static int g_cap = 1024;

template<class T>
class RingQueue
{
public:
	RingQueue(const int cap = g_cap);
	void push(const T& in);
	void pop(T* out);
	size_t size();
	bool empty();
private:
	std::vector<T> _queue;
	std::mutex _mutex;		// protect pstep/cstep/count
	std::counting_semaphore<g_cap> _psem;
	std::counting_semaphore<g_cap> _csem;
	int _pstep;
	int _cstep;
	int _count;
};

template<class T>
RingQueue<T>::RingQueue(const int cap)
	:_queue(cap), _psem(cap), _csem(0), _pstep(0), _cstep(0),_count(0)
{
}

template<class T>
void RingQueue<T>::push(const T& in)
{
	_psem.acquire();
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue[_pstep++] = in;
		_pstep %= _queue.size();
		_count++;
	}
	_csem.release();
}

template<class T>
void RingQueue<T>::pop(T* out)
{
	_csem.acquire();
	{
		std::lock_guard<std::mutex> lock(_mutex);
		*out = _queue[_cstep++];
		_cstep %= _queue.size();
		_count--;
	}
	_psem.release();
}

template<class T>
size_t RingQueue<T>::size()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _count;
}

template<class T>
bool RingQueue<T>::empty()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _count == 0;
}
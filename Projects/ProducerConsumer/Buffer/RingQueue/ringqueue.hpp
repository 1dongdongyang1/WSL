/*
开发日志
1. 接口设计
   环形队列是给生产者-消费者模型和线程池提供缓冲区，本质上还是作为队列，提供给上层接口
   需要考虑的是多线程并发问题
2. 底层数据结构 - 数组/链表 -> 当前文件采用数组
   逻辑上是环形队列
   数组：1. cache命中率高   2. 索引运算简单
   链表：1. cache命中率低   2. 实现复杂
3. 同步机制
   同一时间只能一个生产者能放入     -> 一个锁
   同一时间只能一个消费者能取出     -> 一个锁
   同一时间放入和取出可以同时进行，但是不能同时修改同一个位置         -> 一个锁 / 信号量 / 条件变量
                                    不推荐直接对着数组加锁，锁的粒度大 -> 采用信号量
4. 注意：
    1. 环形队列是模板类，声明定义不能拆开文件写
    2. -std=c++20
    3. 对比队列，环形队列最大的问题是无法扩容
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
    void pop();
    T* back();
    T* front();
    size_t size();
    bool empty();
private:
    std::vector<T> _queue;
    std::mutex _pmutex;
    std::mutex _cmutex;
    std::counting_semaphore<g_cap> _psem;
    std::counting_semaphore<g_cap> _csem;
    int _pstep;
    int _cstep;
};

template<class T>
RingQueue<T>::RingQueue(const int cap)
    :_queue(cap), _pmutex(), _cmutex(), _psem(cap), _csem(0), _pstep(0), _cstep(0)
{
}

template<class T>
void RingQueue<T>::push(const T& in)
{
    _psem.acquire();
    {
        std::lock_guard<std::mutex> lock(_pmutex);
        _queue[_pstep++] = in;
        _pstep %= _queue.size();
    }
    _csem.release();
}

template<class T>
void RingQueue<T>::pop()
{
    _csem.acquire();
    ++_cstep %= _queue.size();
    _psem.release();
}

template<class T>
T* RingQueue<T>::back()
{
    std::lock_guard<std::mutex> lock(_cmutex);
    return _queue[_cstep];  // 环形队列里本身就是存的地址
}

template<class T>
T* RingQueue<T>::front()
{
    std::lock_guard<std::mutex> lock(_cmutex);
    return _queue[_pstep];
}

template<class T>
size_t RingQueue<T>::size()
{
    return _queue.size();
}

template<class T>
bool RingQueue<T>::empty()
{
    if (_pstep != _cstep) return false;
    if (_psem > 0) return true;
    else return false;
}
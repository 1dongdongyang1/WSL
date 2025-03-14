#pragma once

#include "LockGuard.hpp"
#include "Thread.hpp"
#include <unistd.h>
#include <vector>
#include <queue>

using namespace ThreadNs;

const int gnum = 5;

template<class T>
class ThreadPool
{
private:
    static void* handlerTask(void* args)
    {
        ThreadPool<T>* _this = static_cast<ThreadPool<T>*>(args);
        while (true)
        {
            T t;
            {
                LockGuard lg(_this->getMutex());
                while (_this->isEmpty()) _this->threadWait();
                t = _this->pop();
            }
            std::cout << "计算: " << t() << std::endl;
        }
    }
public:
    pthread_mutex_t* getMutex() { return &_mutex; }
    bool isEmpty() { return _task_queue.empty(); }
    void threadWait() { pthread_cond_wait(&_cond, &_mutex); }
    T pop()
    {
        T t = _task_queue.front();
        _task_queue.pop();
        return t;
    }
public:
    ThreadPool(const int& num = gnum) :_num(num)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
        for (int i = 0; i < num; i++)
        {
            _threads.push_back(new Thread(handlerTask, this));
        }
    }
    void run()
    {
        for (const auto& t : _threads)
        {
            t->start();
            std::cout << t->threadname() << " start ..." << std::endl;
        }
    }
    void push(const T& in)
    {
        LockGuard lg(&_mutex);
        _task_queue.push(in);
        pthread_cond_signal(&_cond);
    }
    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
        for (const auto& t : _threads) delete t;
    }
private:
    int _num;
    std::vector<Thread*> _threads;
    std::queue<T> _task_queue;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
};
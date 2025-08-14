#pragma once

#include <iostream>
#include <pthread.h>
#include <queue>

// const int gmaxcap = 5;

template<class T>
class BlockQueue
{
public:
    static const int gmaxcap;
public:
    BlockQueue(const int& maxcap = gmaxcap) :_maxcap(maxcap)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_ccond, nullptr);
        pthread_cond_init(&_pcond, nullptr);
    }
    void push(const T& in)
    {
        pthread_mutex_lock(&_mutex);
        while (is_full()) //while做判断
        {
            pthread_cond_wait(&_pcond, &_mutex);    // 挂起时释放锁，醒来竞争锁 ? 多线程情况?
        }
        _q.push(in);
        pthread_cond_signal(&_ccond);
        pthread_mutex_unlock(&_mutex);
    }
    void pop(T* out)
    {
        pthread_mutex_lock(&_mutex);
        while (is_empty())
        {
            pthread_cond_wait(&_ccond, &_mutex);
        }
        *out = _q.front();
        _q.pop();
        pthread_cond_signal(&_pcond);
        pthread_mutex_unlock(&_mutex);
    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_ccond);
        pthread_cond_destroy(&_pcond);
    }
private:
    bool is_empty()
    {
        return _q.empty();
    }
    bool is_full()
    {
        return _q.size() == _maxcap;
    }
private:
    std::queue<T> _q;
    int _maxcap;
    pthread_mutex_t _mutex;
    pthread_cond_t _ccond;
    pthread_cond_t _pcond;
};

template <class T>
const int BlockQueue<T>::gmaxcap = 5;
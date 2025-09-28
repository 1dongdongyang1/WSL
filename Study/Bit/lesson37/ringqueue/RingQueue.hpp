#pragma once

#include <iostream>
#include <vector>
#include <semaphore.h>
#include <pthread.h>
#include <cassert>

static const int gcap = 5;

template<class T>
class RingQueue
{
public:
    RingQueue(const int& cap = gcap):_queue(cap)
    {
        int n = sem_init(&_psem, 0, cap);
        assert(n == 0);
        n = sem_init(&_csem, 0, 0);
        assert(n == 0);

        _pstep = _cstep = 0;
        pthread_mutex_init(&_pmutex, nullptr);
        pthread_mutex_init(&_cmutex, nullptr);
    }
    ~RingQueue()
    {
        sem_destroy(&_psem);
        sem_destroy(&_csem);

        pthread_mutex_destroy(&_pmutex);
        pthread_mutex_destroy(&_cmutex);
    }
    void push(const T& in)
    {
        P(_psem);
        pthread_mutex_lock(&_pmutex);
        _queue[_pstep++] = in;
        _pstep %= _queue.size();
        pthread_mutex_unlock(&_pmutex);
        V(_csem);
    }
    void pop(T* out)
    {
        P(_csem);
        pthread_mutex_lock(&_cmutex);
        *out = _queue[_cstep++];
        _cstep %= _queue.size();
        pthread_mutex_unlock(&_cmutex);
        V(_psem);
    }
private:
    void P(sem_t& sem)
    {
        int n = sem_wait(&sem);
        assert(n == 0);
        (void)n;
    }
    void V(sem_t& sem)
    {
        int n = sem_post(&sem);
        assert(n == 0);
        (void)n;
    }
private:
    std::vector<T> _queue;
    sem_t _psem;
    sem_t _csem;
    int _pstep;
    int _cstep;
    pthread_mutex_t _pmutex;
    pthread_mutex_t _cmutex;
};
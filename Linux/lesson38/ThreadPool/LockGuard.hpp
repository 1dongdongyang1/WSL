#pragma once

#include <iostream>
#include <pthread.h>

class Mutex
{
public:
    Mutex(pthread_mutex_t* lock_p = nullptr): _lock_p(lock_p){}
    ~Mutex(){}
    void lock(){if (_lock_p) pthread_mutex_lock(_lock_p);}
    void unlock(){if (_lock_p) pthread_mutex_unlock(_lock_p);}
private:
    pthread_mutex_t* _lock_p;
};

class LockGuard
{
public:
    LockGuard(pthread_mutex_t* lock_p): _mutex(lock_p){_mutex.lock();}
    ~LockGuard(){_mutex.unlock();}
private:
    Mutex _mutex;
};
#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <pthread.h>
#include <cassert>

namespace ThreadNs
{
    typedef std::function<void* (void*)> func_t;
    const int num = 1024;

    class Thread
    {
    private:
        static void* start_routine(void* args)
        {
            Thread* _this = static_cast<Thread*>(args);
            return _this->callback();
        }
    public:
        Thread(func_t func, void* args = nullptr)
            :_func(func), _args(args)
        {
            char buffer[num];
            snprintf(buffer, sizeof buffer, "thread-%d", _threadnum++);
            _name = buffer;
        }
        void start()
        {
            int n = pthread_create(&_tid, nullptr, start_routine, this);
            assert(n == 0);
            (void)n;
        }
        void join()
        {
            int n = pthread_join(_tid, nullptr);
            assert(n == 0);
            (void)n;
        }
        std::string threadname()
        {
            return _name;
        }
        ~Thread() {}
        void* callback() { return _func(_args); }
    private:
        std::string _name;
        pthread_t _tid;
        func_t _func;
        void* _args;

        static int _threadnum;
    };

    int Thread::_threadnum = 1;
}
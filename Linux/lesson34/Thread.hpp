#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <pthread.h>
#include <cassert>

class Thread;

class Context
{
public:
    Thread* _this;
    void* _args;

public:
    Context()
        : _this(nullptr), _args(nullptr)
    {
    }

    ~Context()
    {
    }
};

class Thread
{
public:
    typedef std::function<void* (void*)> func_t;
    const int num = 1024;

public:
    Thread(func_t func, void* args, int number)
        : _func(func), _args(args)
    {
        char buffer[num];
        snprintf(buffer, sizeof buffer, "thread-%d", number);
        _name = buffer;
    }

    void* run(void* args)
    {
        return _func(_args);
    }

    static void* start_routine(void* args)
    {
        Context* ctx = static_cast<Context*>(args);
        void* ret = ctx->_this->run(ctx->_args);
        delete ctx;
        return ret;
    }

    void start()
    {
        Context* ctx = new Context();
        ctx->_this = this;
        ctx->_args = _args;
        int n = pthread_create(&_tid, nullptr, start_routine, ctx);
        assert(n == 0);
        (void)n;
    }

    void join()
    {
        int n = pthread_join(_tid, nullptr);
        assert(n == 0);
        (void)n;
    }

private:
    std::string _name;
    pthread_t _tid;
    func_t _func;
    void* _args;
};
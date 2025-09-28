#pragma once

#include <iostream>
#include <functional>

class CalTask
{
public:
    using func_t = std::function<int(int, int)>;
public:
    CalTask()
    {}
    CalTask(int x, int y, func_t func) :_x(x), _y(y), _callback(func)
    {}
    int operator()()
    {
        return _callback(_x, _y);
    }
private:
    int _x;
    int _y;
    func_t _callback;
};

int Add(int x, int y)
{
    return x + y;
}

class SaveTask
{
public:
    using func_t = std::function<void(const std::string&)>;
public:
    SaveTask()
    {
    }
    SaveTask(const std::string& message, func_t func) :_message(message), _func(func)
    {
    }
    void operator()()
    {
        _func(_message);
    }
private:
    std::string _message;
    func_t _func;
};

void Save(const std::string& message)
{
    std::string target = "./log.txt";
    FILE* fp = fopen(target.c_str(), "a+");
    fputs(message.c_str(), fp);
    fputs("\n", fp);
    fclose(fp);
}
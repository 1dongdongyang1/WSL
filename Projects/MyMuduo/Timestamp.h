#pragma once

#include <iostream>
#include <string>

// 时间类
class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    
    static Timestamp now();     // 注意这里不是单例
    std::string toString() const;
private:
    int64_t microSecondsSinceEpoch_;
};
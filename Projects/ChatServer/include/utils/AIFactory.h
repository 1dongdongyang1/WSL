#pragma once

#include "utils/AIStrategy.h"

#include <memory>
#include <functional>

class StrategyFactory {
public:
    using Creator = std::function<std::shared_ptr<AIStrategy>()>;

    static StrategyFactory& instance();
    void registerStrategy(const std::string& name, Creator creator);
    std::shared_ptr<AIStrategy> createStrategy(const std::string& name);

private:
    StrategyFactory() = default;
    std::unordered_map<std::string, Creator> creators_;
};

template<typename T>
struct StrategyRegister {
    StrategyRegister(const std::string& name) {
        StrategyFactory::instance().registerStrategy(name, []() {
            return std::make_shared<T>();
        });
    }
};
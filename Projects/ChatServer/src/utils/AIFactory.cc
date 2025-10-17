#include "utils/AIFactory.h"

StrategyFactory& StrategyFactory::instance() {
    static StrategyFactory instance;
    return instance;
}

void StrategyFactory::registerStrategy(const std::string& name, Creator creator) {
    creators_[name] = std::move(creator);
}

std::shared_ptr<AIStrategy> StrategyFactory::createStrategy(const std::string& name) {
    auto it = creators_.find(name);
    if (it == creators_.end()) {
        throw std::runtime_error("Strategy not found: " + name);
    }
    return it->second();
}
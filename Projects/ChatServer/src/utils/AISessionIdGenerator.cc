#include "utils/AISessionIdGenerator.h"

#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>

AISessionIdGenerator::AISessionIdGenerator() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

std::string AISessionIdGenerator::generate() {
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    long long randomPart = std::rand() % 1000000; 
    long long rawId = now ^ randomPart;
    return std::to_string(rawId);
}
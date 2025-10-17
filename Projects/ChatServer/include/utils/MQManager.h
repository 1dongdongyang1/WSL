#pragma once

#include <mutex>
#include <thread>
#include <atomic>
#include <string>
#include <memory>
#include <vector>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

class MQManager {
public:
    static MQManager& instance();
    void publish(const std::string& queueName, const std::string& message);

private:
    struct MQConn {
        AmqpClient::Channel::ptr_t channel;
        std::mutex mutex;
    };
    MQManager(size_t poolSize = 5);

    MQManager(const MQManager&) = delete;
    MQManager& operator=(const MQManager&) = delete;
    MQManager(MQManager&&) = delete;
    MQManager& operator=(MQManager&&) = delete;

private:
    std::vector<std::shared_ptr<MQConn>> pool_;
    size_t poolSize_;
    std::atomic<size_t> counter_;
};

class RabbitMQThreadPool {
public:
    using HandlerFunc = std::function<void(const std::string& message)>;

    RabbitMQThreadPool(const std::string& host,
        const std::string& queueName,
        int threadNum,
        HandlerFunc handler);
    ~RabbitMQThreadPool();
    void start();
    void shutdown();

private:
    void worker(int id);

private:
    std::vector<std::thread> workers_;
    std::atomic<bool> stop_;
    std::string queueName_;
    int threadNum_;
    HandlerFunc handler_;
    std::string rabbitmqHost_;
};
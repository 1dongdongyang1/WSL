#include "utils/MQManager.h"
#include <muduo/base/Logging.h>

MQManager& MQManager::instance() {
    static MQManager instance;
    return instance;
}

void MQManager::publish(const std::string& queueName, const std::string& message) {
    size_t index = counter_++ % poolSize_;
    auto& conn = pool_[index];

    std::lock_guard<std::mutex> lock(conn->mutex);
    AmqpClient::BasicMessage::ptr_t msg = AmqpClient::BasicMessage::Create(message);
    conn->channel->BasicPublish("", queueName, msg);
}

MQManager::MQManager(size_t poolSize)
    : poolSize_(poolSize), counter_(0) {
    for (size_t i = 0; i < poolSize_; ++i) {
        auto conn = std::make_shared<MQConn>();
        conn->channel = AmqpClient::Channel::Create("localhost", 5672, "ddy", "123456", "/");
        pool_.push_back(conn);
    }
}

RabbitMQThreadPool::RabbitMQThreadPool(const std::string& host,
    const std::string& queueName,
    int threadNum,
    HandlerFunc handler)
    : rabbitmqHost_(host),
    queueName_(queueName),
    threadNum_(threadNum),
    handler_(handler),
    stop_(false) {
}

RabbitMQThreadPool::~RabbitMQThreadPool() {
    shutdown();
}

void RabbitMQThreadPool::start() {
    for (int i = 0; i < threadNum_; ++i) {
        workers_.emplace_back(&RabbitMQThreadPool::worker, this, i);
    }
}

void RabbitMQThreadPool::shutdown() {
    stop_ = true;
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers_.clear();
}

void RabbitMQThreadPool::worker(int id) {
    try {
        auto channel = AmqpClient::Channel::Create(rabbitmqHost_, 5672, "ddy", "123456", "/");
        channel->DeclareQueue(queueName_, false, true, false, false); // durable, non-exclusive, non-auto-delete
        std::string consumerTag = channel->BasicConsume(queueName_, "", true, false, false); // no-local, no-ack, exclusive
        channel->BasicQos(consumerTag, 1); // prefetch count = 1

        while (!stop_) {
            AmqpClient::Envelope::ptr_t envelope;
            bool received = channel->BasicConsumeMessage(consumerTag, envelope, 5); // timeout 5 seconds
            if (received && envelope) {
                std::string message = envelope->Message()->Body();
                handler_(message);
                channel->BasicAck(envelope);
            }
        }
        channel->BasicCancel(consumerTag);
    }
    catch (const std::exception& e) {
        LOG_ERROR << "RabbitMQ worker " << id << " encountered an error: " << e.what();
    }
}
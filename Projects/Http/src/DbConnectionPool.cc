#include "DbConnection.h"
#include "DbConnectionPool.h"

#include <muduo/base/Logging.h>

namespace http {
    namespace db {
        void DbConnectionPool::init(const std::string& host,
                                 const std::string& user,
                                 const std::string& password,
                                 const std::string& database,
                                 size_t poolSize) {
            // 线程池会被多线程调用，需加锁
            std::lock_guard<std::mutex> lock(mutex_);

            // 确保只初始化一次
            if (init_) {
                return;
            }

            host_ = host;
            user_ = user;
            password_ = password;
            database_ = database;

            // 创建连接
            for (size_t i = 0; i < poolSize; ++i) {
                connections_.push(createConnection());
            }

            init_ = true;
            LOG_INFO << "DbConnectionPool initialized with " << poolSize << " connections.";
        }


        DbConnectionPool::DbConnectionPool() {
            // 启动连接检查线程
            checkThread_ = std::thread(&DbConnectionPool::connectionChecker, this);
            checkThread_.detach();
        }

        DbConnectionPool::~DbConnectionPool() {
            std::lock_guard<std::mutex> lock(mutex_);
            while (!connections_.empty()) {
                connections_.pop();
            }
            init_ = false;
            LOG_INFO << "DbConnectionPool destroyed.";
        }

        std::shared_ptr<DbConnection> DbConnectionPool::getConnection() {
            std::shared_ptr<DbConnection> conn;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]() { return !connections_.empty(); });
                conn = connections_.front();
                connections_.pop();
            }
            try {
                if(!conn->ping()) {
                    conn->reconnect();
                }
                return std::shared_ptr<DbConnection>(conn.get(), [this](DbConnection* p) {
                    std::lock_guard<std::mutex> lock(mutex_);
                    connections_.push(std::shared_ptr<DbConnection>(p));
                    cv_.notify_one();
                });
            }
            catch (const std::exception& e) {
                LOG_ERROR << "Failed to get valid connection: " << e.what();
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    connections_.push(createConnection());
                    cv_.notify_one();
                }
                throw DbException(e.what());
            }
        }

        std::shared_ptr<DbConnection> DbConnectionPool::createConnection() {
            try {
                auto conn = std::make_shared<DbConnection>(host_, user_, password_, database_);
                return conn;
            } catch (const std::exception& e) {
                LOG_ERROR << "Failed to create connection: " << e.what();
                throw DbException(e.what());
            }
        }

        void DbConnectionPool::connectionChecker() {
            while (true) {
                try {
                    std::vector<std::shared_ptr<DbConnection>> connsToCheck;
                    {
                        std::lock_guard<std::mutex> lock(mutex_);
                        if(connections_.empty()) {
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                            continue;
                        }
                        auto tmp = connections_;
                        while (!tmp.empty()) {
                            connsToCheck.push_back(tmp.front());
                            tmp.pop();
                        }
                    }

                    // 在锁外检查连接
                    for (auto& conn : connsToCheck) {
                        if (!conn->ping()) {
                            try {
                                conn->reconnect();
                            } catch (const std::exception& e) {
                                LOG_ERROR << "Failed to reconnect: " << e.what();
                            }
                        }
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(60));
                }
                catch (const std::exception& e) {
                    LOG_ERROR << "Connection checker error: " << e.what();
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            }
        }
    }
}
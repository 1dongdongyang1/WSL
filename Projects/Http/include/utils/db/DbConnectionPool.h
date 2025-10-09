#pragma once

#include <queue>
#include <mutex>
#include <memory>
#include <thread>
#include <string>
#include <condition_variable>

#include "DbConnection.h"

namespace http {
    namespace db {
        class DbConnectionPool {
        public:
            // 单例
            static DbConnectionPool& getInstance() {
                static DbConnectionPool instance;
                return instance;
            }

            // 初始化连接池
            void init(const std::string& host,
                      const std::string& user,
                      const std::string& password,
                      const std::string& database,
                      size_t poolSize = 10);

            // 获取数据库连接
            std::shared_ptr<DbConnection> getConnection();

        private:
            DbConnectionPool();
            ~DbConnectionPool();

            // 禁止拷贝和赋值
            DbConnectionPool(const DbConnectionPool&) = delete;
            DbConnectionPool& operator=(const DbConnectionPool&) = delete;

            std::shared_ptr<DbConnection> createConnection();
            void connectionChecker();

        private:
            std::queue<std::shared_ptr<DbConnection>>   connections_;
            std::mutex                                  mutex_;
            std::condition_variable                     cv_;
            std::string                                 host_;
            std::string                                 user_;
            std::string                                 password_;
            std::string                                 database_;
            bool                                        init_ = false;
            std::thread                                 checkThread_;
        };
    }
}


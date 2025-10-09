#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <mysql/mysql.h>
#include <mysql_driver.h>
#include <muduo/base/Logging.h>

#include "DbException.h"

namespace http {
    namespace db {
        class DbConnection {
        public:
            DbConnection(const std::string& host,
                         const std::string& user,
                         const std::string& password,
                         const std::string& database);
            ~DbConnection();

            // 禁止拷贝和赋值
            DbConnection(const DbConnection&) = delete;
            DbConnection& operator=(const DbConnection&) = delete;

            // 检查连接是否有效
            bool ping(); 
            void reconnect();
            void cleanup();

            template<typename... Args>
            sql::ResultSet* executeQuery(const std::string& query, Args&&... args) {
                std::lock_guard<std::mutex> lock(mutex_);
                try {
                    std::unique_ptr<sql::PreparedStatement> pstmt(connection_->prepareStatement(query));
                    bindParams(pstmt.get(), 1, std::forward<Args>(args)...);
                    return pstmt->executeQuery();
                } catch (const std::exception& e) {
                    LOG_ERROR << "Query failed: " << e.what() << ", SQL: " << query;
                    throw DbException(e.what());
                }
            }

            template<typename... Args>
            int executeUpdate(const std::string& query, Args&&... args) {
                std::lock_guard<std::mutex> lock(mutex_);
                try {
                    std::unique_ptr<sql::PreparedStatement> pstmt(connection_->prepareStatement(query));
                    bindParams(pstmt.get(), 1, std::forward<Args>(args)...);
                    return pstmt->executeUpdate();
                } catch (const std::exception& e) {
                    LOG_ERROR << "Update failed: " << e.what() << ", SQL: " << query;
                    throw DbException(e.what());
                }
            }

        private:
            // 递归中止条件
            void bindParams(sql::PreparedStatement* pstmt, int index) {}

            template<typename T, typename... Args>
            void bindParams(sql::PreparedStatement* pstmt, int index, T&& value, Args&&... args) {
                pstmt->setString(index, std::to_string(std::forward<T>(value)));
                bindParams(pstmt, index + 1, std::forward<Args>(args)...);
            }

            // 特化字符串类型
            template<typename... Args>
            void bindParams(sql::PreparedStatement* pstmt, int index, const std::string& value, Args&&... args) {
                pstmt->setString(index, value);
                bindParams(pstmt, index + 1, std::forward<Args>(args)...);
            }

        private:
            std::shared_ptr<sql::Connection>    connection_;
            std::string                         host_;
            std::string                         user_;
            std::string                         password_;
            std::string                         database_;
            std::mutex                          mutex_;     
        };
    }
}
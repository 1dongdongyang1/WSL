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

#include "utils/db/DbException.h"

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
                }
                catch (const std::exception& e) {
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
                }
                catch (const std::exception& e) {
                    LOG_ERROR << "Update failed: " << e.what() << ", SQL: " << query;
                    throw DbException(e.what());
                }
            }

        private:
            // 递归终止
            inline void bindParams(sql::PreparedStatement* pstmt, int index) {}

            // 泛型模板
            template<typename T, typename... Args>
            void bindParams(sql::PreparedStatement* pstmt, int index, T&& value, Args&&... args) {
                using Decayed = std::decay_t<T>;

                if constexpr (std::is_same_v<Decayed, std::string>) {
                    // 所有 string 类型
                    pstmt->setString(index, std::forward<T>(value));
                }
                else if constexpr (std::is_arithmetic_v<Decayed>) {
                    // 数值类型
                    pstmt->setString(index, std::to_string(std::forward<T>(value)));
                }
                else if constexpr (std::is_same_v<Decayed, const char*> || std::is_same_v<Decayed, char*>) {
                    // C 风格字符串
                    pstmt->setString(index, std::string(value));
                }
                else if constexpr (std::is_same_v<Decayed, std::chrono::system_clock::time_point>) {
                    auto t_c = std::chrono::system_clock::to_time_t(value);
                    pstmt->setString(index, std::ctime(&t_c));
                }
                else {
                    static_assert(always_false<Decayed>::value, "Unsupported parameter type in bindParams");
                }

                bindParams(pstmt, index + 1, std::forward<Args>(args)...);
            }

            // 辅助模板，用于 static_assert
            template<typename>
            struct always_false : std::false_type {};


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
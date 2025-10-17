#pragma once

#include "utils/db/DbConnectionPool.h"

#include <string>

namespace http {
    class MysqlUtil {
    public:
        static void init(const std::string& host,
            const std::string& user,
            const std::string& password,
            const std::string& database,
            size_t poolSize = 10) {
            db::DbConnectionPool::instance().init(host, user, password, database, poolSize);
        }

        template<typename... Args>
        sql::ResultSet* executeQuery(const std::string& query, Args&&... args) {
            auto conn = db::DbConnectionPool::instance().getConnection();
            return conn->executeQuery(query, std::forward<Args>(args)...);
        }

        template<typename... Args>
        int executeUpdate(const std::string& query, Args&&... args) {
            auto conn = db::DbConnectionPool::instance().getConnection();
            return conn->executeUpdate(query, std::forward<Args>(args)...);
        }
    };
}
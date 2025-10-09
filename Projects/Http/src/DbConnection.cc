#include "DbConnection.h"
#include <muduo/base/Logging.h>

namespace http {
    namespace db {
        DbConnection::DbConnection(const std::string& host,
            const std::string& user,
            const std::string& password,
            const std::string& database)
            : host_(host)
            , user_(user)
            , password_(password)
            , database_(database) {
            try {
                sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
                connection_.reset(driver->connect(host, user, password));
                if (connection_) {
                    connection_->setSchema(database_);

                    // 设置连接属性
                    connection_->setClientOption("OPT_RECONNECT", "true");      // 自动重连
                    connection_->setClientOption("OPT_CONNECT_TIMEOUT", "10");  // 连接超时10秒
                    connection_->setClientOption("mult_statements", "false");   // 禁用多语句

                    // 设置字符集
                    std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
                    stmt->execute("SET NAMES utf8mb4");

                    LOG_INFO << "Database connected to " << host << " successfully.";
                }
            }
            catch (const std::exception& e) {
                LOG_ERROR << "Database connection failed: " << e.what();
                throw DbException(e.what());
            }
        }

        DbConnection::~DbConnection() {
            try {
                cleanup();
            }
            catch (...) {
                // 忽略析构函数中的异常
            }
            LOG_INFO << "DbConnection destroyed.";
        }

        bool DbConnection::ping() {
            try {
                std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
                std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 1"));
                return true;
            }
            catch (const std::exception& e) {
                LOG_ERROR << "Ping failed: " << e.what();
                return false;
            }
        }

        void DbConnection::reconnect() {
            try {
                if (connection_) {
                    connection_->reconnect();
                }
                else {
                    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
                    connection_.reset(driver->connect(host_, user_, password_));
                    connection_->setSchema(database_);
                }
            }
            catch (const std::exception& e) {
                LOG_ERROR << "Reconnection failed: " << e.what();
                throw DbException(e.what());
            }
        }

        void DbConnection::cleanup() {
            std::lock_guard<std::mutex> lock(mutex_);
            try {
                if (connection_) {
                    // 确保所有的事物都被提交
                    if (!connection_->getAutoCommit()) {
                        connection_->rollback();
                        connection_->setAutoCommit(true);
                    }

                    // 清理所有未处理的结果集
                    std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
                    while (stmt->getMoreResults()) {
                        auto res = stmt->getResultSet();
                        while (res && res->next()) {
                            // 仅遍历结果集以清理
                        }
                    }
                }
            }
            catch (const std::exception& e) {
                LOG_WARN << "Cleanup failed: " << e.what();
                try {
                    reconnect();
                }
                catch (...) {
                    // 忽略重连中的异常
                }
            }
        }
    }
}

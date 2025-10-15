#include "session/DBSessionStorage.h"
#include "utils/MysqlUtil.h"
#include <nlohmann/json.hpp>
#include <muduo/base/Logging.h>

namespace http {

    namespace session {

        std::string formatTimePointForMySQL(const std::chrono::system_clock::time_point& tp) {
            std::time_t t = std::chrono::system_clock::to_time_t(tp);
            std::tm tm{};
            localtime_r(&t, &tm); // 或 gmtime_r(&t, &tm) 根据需要

            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
            return oss.str();
        }

        std::shared_ptr<Session> DBSessionStorage::load(const std::string& sessionId) {
            // DB-> memory
            auto rs = MysqlUtil().executeQuery(
                "SELECT data FROM sessions WHERE session_id=? AND expires_at > NOW()",
                sessionId
            );
            if (!rs || !rs->next()) {
                return nullptr; // 会话不存在或已过期
            }

            std::string jsonStr = rs->getString("data");
            auto j = nlohmann::json::parse(jsonStr);
            std::unordered_map<std::string, std::string> data = j.get<std::unordered_map<std::string, std::string>>();
            auto session = std::make_shared<Session>(sessionId, nullptr, expiryTime_);
            for (const auto& [key, value] : data) {
                session->set(key, value);
            }
            return session;
        }

        void DBSessionStorage::save(std::shared_ptr<Session> session) {
            // memory -> DB
            std::string sessionId = session->sessionId();
            nlohmann::json j = session->data();
            std::string jsonStr = j.dump();
            std::string expireTime = formatTimePointForMySQL(session->expiryTime());
            MysqlUtil().executeUpdate(
                "INSERT INTO sessions (session_id, data, expires_at) VALUES (?, ?, ?) "
                "ON DUPLICATE KEY UPDATE data=?, expires_at=?",
                sessionId, jsonStr, expireTime, jsonStr, expireTime
            );
        }

        void DBSessionStorage::remove(const std::string& sessionId) {
            MysqlUtil().executeUpdate(
                "DELETE FROM sessions WHERE session_id=?",
                sessionId
            );
        }

        bool DBSessionStorage::exists(const std::string& sessionId) {
            auto rs = MysqlUtil().executeQuery(
                "SELECT COUNT(*) FROM sessions WHERE session_id=? AND expires_at > NOW()",
                sessionId
            );
            return rs && rs->next();
        }

        std::string DBSessionStorage::escapeString(const std::string& str) {
            std::string out;
            for (char c : str) {
                if (c == '\'') out += "\\'";
                else if (c == '\"') out += "\\\"";
                else if (c == '\\') out += "\\\\";
                else out += c;
            }
            return out;
        }

    }
}
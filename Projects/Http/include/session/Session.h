#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <chrono>

namespace http {

    namespace session {

        class SessionManager;

        class Session : public std::enable_shared_from_this<Session> {
        public:
            Session(const std::string& sessionId, SessionManager* sessionManager, int maxAge = 3600); // 默认1小时

            bool isExpired() const;
            void refresh(); // 刷新最后访问时间

            // get
            std::string sessionId() const { return sessionId_; }
            SessionManager* sessionManager() const { return sessionManager_; }

            // set
            void setManager(SessionManager* manager) { sessionManager_ = manager; }

            // 数据操作
            void set(const std::string& key, const std::string& value);
            std::string get(const std::string& key) const;
            void remove(const std::string& key);
            void clear();

        private:
            std::string                                     sessionId_;
            SessionManager*                                 sessionManager_;
            int                                             maxAge_;            // 以秒为单位
            std::unordered_map<std::string, std::string>    data_;
            std::chrono::system_clock::time_point           lastAccessed_;
        };

    } // namespace session

} // namespace http
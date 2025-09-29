#pragma once

#include <string>
#include <memory> 
#include <random>

#include "SessionStorage.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace http {

    namespace session {

        class SessionManager {
        public:
            explicit SessionManager(std::unique_ptr<SessionStorage> storage);

            std::shared_ptr<Session> getSession(const HttpRequest& request, HttpResponse* response);
            void destroySession(const std::string& sessionId);
            void cleanExpiredSessions();
            void updateSession(std::shared_ptr<Session> session);

        private:
            // 生成唯一的会话ID
            std::string generateSessionId();
            // 从请求的Cookie中提取会话ID
            std::string getSessionIdFromCookie(const HttpRequest& request);
            void setSessionIdCookie(const std::string& sessionId, HttpResponse* response);

        private:
            std::unique_ptr<SessionStorage> storage_;
            std::mt19937 rng_;  // 随机数生成器
        };

    } // namespace session
}
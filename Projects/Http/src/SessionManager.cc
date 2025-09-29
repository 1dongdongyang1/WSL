#include "SessionManager.h"

namespace http {

    namespace session {

        SessionManager::SessionManager(std::unique_ptr<SessionStorage> storage)
            : storage_(std::move(storage))
            , rng_(std::random_device{}())  // 使用随机设备初始化随机数生成器
        {
        }

        std::shared_ptr<Session> SessionManager::getSession(const HttpRequest& request, HttpResponse* response) {
            std::string sessionId = getSessionIdFromCookie(request);
            std::shared_ptr<Session> session;

            // 有效的 sessionId 则尝试加载会话
            if (!sessionId.empty()) {
                session = storage_->load(sessionId);
            }

            // 如果没有会话，或者会话已过期，则创建新会话
            if (!session || session->isExpired()) {
                sessionId = generateSessionId();
                session = std::make_shared<Session>(sessionId, this);
                setSessionIdCookie(sessionId, response);
            }
            else {
                // TODO: 为什么这里还要设置 manager？
                // 可能是因为从存储加载的 session 没有设置 manager
                session->setManager(this);
            }

            session->refresh();
            storage_->save(session);
            return session;
        }

        void SessionManager::destroySession(const std::string& sessionId) {
            storage_->remove(sessionId);
        }

        void SessionManager::cleanExpiredSessions() {
            // 注意：这个实现依赖于具体的存储实现
            // 对于内存存储，可以在加载时检查是否过期
            // 对于其他存储的实现，可能需要定期清理过期会话
        }

        void SessionManager::updateSession(std::shared_ptr<Session> session) {
            storage_->save(session);
        }

        std::string SessionManager::generateSessionId() {
            static const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
            static const size_t length = 32;

            std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
            std::string sessionId;
            for (size_t i = 0; i < length; ++i) {
                sessionId += charset[dist(rng_)];
            }
            return sessionId;
        }

        std::string SessionManager::getSessionIdFromCookie(const HttpRequest& request) {
            std::string sessionId;
            std::string cookie = request.getHeader("Cookie");
            if (!cookie.empty()) {
                size_t pos = cookie.find("sessionId=");
                if (pos != std::string::npos) {
                    size_t end = cookie.find(';', pos);
                    if (end != std::string::npos) {
                        sessionId = cookie.substr(pos + 10, end - pos - 10);
                    }
                    else {
                        sessionId = cookie.substr(pos + 10);
                    }
                }
            }
            return sessionId;
        }
        void SessionManager::setSessionIdCookie(const std::string& sessionId, HttpResponse* response) {
            std::string cookie = "sessionId=" + sessionId + "; Path=/; HttpOnly";
            response->addHeader("Set-Cookie", cookie);
        }
    } // namespace session
} // namespace http
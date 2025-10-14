#include "Session.h"
#include "SessionManager.h"

namespace http {

    namespace session {

        Session::Session(const std::string& sessionId, SessionManager* sessionManager, int maxAge)
            : sessionId_(sessionId)
            , sessionManager_(sessionManager)
            , maxAge_(maxAge)
            , expiryTime_(std::chrono::system_clock::now() + std::chrono::seconds(maxAge)) {
        }

        bool Session::isExpired() const {
            auto now = std::chrono::system_clock::now();
            return now > expiryTime_;
        }

        void Session::refresh() {
            expiryTime_ = std::chrono::system_clock::now() + std::chrono::seconds(maxAge_);
        }

        void Session::set(const std::string& key, const std::string& value) {
            data_[key] = value;
            if (sessionManager_) {
                sessionManager_->updateSession(shared_from_this());
            }
        }

        std::string Session::get(const std::string& key) const {
            auto it = data_.find(key);
            if (it != data_.end()) {
                return it->second;
            }
            return "";
        }

        void Session::remove(const std::string& key) {
            data_.erase(key);
            // TODO: 这里是否需要通知 SessionManager？
            // 目前设计是需要的，因为数据变更后可能需要持久化
            if (sessionManager_) {
                sessionManager_->updateSession(shared_from_this());
            }
        }

        void Session::clear() {
            data_.clear();
            if (sessionManager_) {
                sessionManager_->updateSession(shared_from_this());
            }
        }
    }

}
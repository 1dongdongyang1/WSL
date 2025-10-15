#pragma once

#include "session/SessionStorage.h"

namespace http {

    namespace session {

        class DBSessionStorage : public SessionStorage {
        public:
            DBSessionStorage(int expiryTime = 3600) : expiryTime_(expiryTime) {}

            std::shared_ptr<Session> load(const std::string& sessionId) override;
            void save(std::shared_ptr<Session> session) override;
            void remove(const std::string& sessionId) override;
            bool exists(const std::string& sessionId) override;

        private:
            std::string escapeString(const std::string& str);

        private:
            int expiryTime_;
        };
    }
}
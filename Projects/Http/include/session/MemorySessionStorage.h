#pragma once

#include <unordered_map>

#include "SessionStorage.h"

namespace http {

    namespace session {

        class MemorySessionStorage : public SessionStorage {
        public:
            std::shared_ptr<Session> load(const std::string& sessionId) override;
            void save(std::shared_ptr<Session> session) override;
            void remove(const std::string& sessionId) override;
            bool exists(const std::string& sessionId) override;
        private:
            std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;
        };

    }
}
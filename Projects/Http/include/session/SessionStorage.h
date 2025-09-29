#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "Session.h"

namespace http {

    namespace session {

        class SessionStorage {
        public:
            virtual ~SessionStorage() = default;
            virtual void save(std::shared_ptr<Session> session) = 0;
            virtual std::shared_ptr<Session> load(const std::string& sessionId) = 0;
            virtual void remove(const std::string& sessionId) = 0;
        };

        class MemorySessionStorage : public SessionStorage {
        public:
            void save(std::shared_ptr<Session> session) override;
            std::shared_ptr<Session> load(const std::string& sessionId) override;
            void remove(const std::string& sessionId) override;
        private:
            std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;
        };
    }
}
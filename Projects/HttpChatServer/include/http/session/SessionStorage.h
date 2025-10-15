#pragma once

#include <string>
#include <memory>

#include "session/Session.h"

namespace http {

    namespace session {

        class SessionStorage {
        public:
            virtual ~SessionStorage() = default;

            virtual std::shared_ptr<Session> load(const std::string& sessionId) = 0;
            virtual void save(std::shared_ptr<Session> session) = 0;
            virtual void remove(const std::string& sessionId) = 0;
            virtual bool exists(const std::string& sessionId) = 0;
        };

    }
}
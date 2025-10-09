#pragma once

#include <memory>

#include "HttpRequest.h"
#include "HttpResponse.h"

namespace http {
    namespace middleware {

        class Middleware {
        public:
            virtual ~Middleware() = default;

            virtual void before(HttpRequest& request) = 0;
            virtual void after(HttpResponse& response) = 0;

            void setNext(std::shared_ptr<Middleware> next) { nextMiddleware_ = next; }

        private:
            std::shared_ptr<Middleware> nextMiddleware_;
        };

    } // namespace middleware
}
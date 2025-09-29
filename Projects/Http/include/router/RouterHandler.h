#pragma once

#include "HttpRequest.h"
#include "HttpResponse.h"

namespace http {
    namespace router {
        class RouterHandler {
        public:
            virtual ~RouterHandler() = default;
            virtual void handle(const HttpRequest& request, HttpResponse* response) = 0;
        };
    }
}
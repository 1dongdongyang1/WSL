#pragma once

#include "http/HttpRequest.h"
#include "http/HttpResponse.h"

namespace http {
    namespace router {
        class RouterHandler {
        public:
            virtual ~RouterHandler() = default;
            virtual void handle(const HttpRequest& request, HttpResponse* response) = 0;
        };
    }
}
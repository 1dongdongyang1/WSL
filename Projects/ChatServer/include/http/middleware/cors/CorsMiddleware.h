#pragma once

#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "middleware/Middleware.h"
#include "middleware/cors/CorsConfig.h"

namespace http {

    namespace middleware {

        class CorsMiddleware : public Middleware {
        public:
            explicit CorsMiddleware(const CorsConfig& config = CorsConfig::defaultConfig());

            void before(HttpRequest& request) override;
            void after(HttpResponse& response) override;

            // Helper function to join vector of strings with a delimiter
            std::string join(const std::vector<std::string>& strings, const std::string& delimiter) const;

        private:
            bool isOriginAllowed(const std::string& origin) const;
            void handlePreflightRequest(const HttpRequest& request, HttpResponse& response);
            void addCorsHeaders(HttpResponse& response, const std::string& origin) const;

        private:
            CorsConfig config_;
        };

    } // namespace middleware
}
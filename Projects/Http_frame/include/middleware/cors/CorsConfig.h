#pragma once

#include <vector>
#include <string>

namespace http {

    namespace middleware {

        struct CorsConfig {
            std::vector<std::string> allowedOrigins;
            std::vector<std::string> allowedMethods;
            std::vector<std::string> allowedHeaders;
            bool allowCredentials = false;
            int maxAge = 3600; // in seconds

            static CorsConfig defaultConfig() {
                return CorsConfig{
                    .allowedOrigins = {"*"},
                    .allowedMethods = {"GET", "POST", "PUT", "DELETE", "OPTIONS"},
                    .allowedHeaders = {"Content-Type", "Authorization"},
                    .allowCredentials = false,
                    .maxAge = 3600
                };
            }
        };

    } // namespace middleware
}
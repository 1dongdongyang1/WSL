#include "CorsMiddleware.h"

#include <algorithm>
#include <sstream>
#include <iostream>
#include <muduo/base/Logging.h>

namespace http {
    namespace middleware {
        CorsMiddleware::CorsMiddleware(const CorsConfig& config)
            : config_(config) {}

        void CorsMiddleware::before(HttpRequest& request) {
            LOG_DEBUG << "CORS Middleware before request";

            if (request.method() == HttpRequest::Method::kOptions) {
                LOG_INFO << "Handling preflight CORS request";
                HttpResponse response;
                handlePreflightRequest(request, response);
                throw response; // Use exception to short-circuit the request handling
            }
        }

        void CorsMiddleware::after(HttpResponse& response) {
            LOG_DEBUG << "CORS Middleware after response";

            if(!config_.allowedOrigins.empty()) {
                if(std::find(config_.allowedOrigins.begin(), config_.allowedOrigins.end(), "*") != config_.allowedOrigins.end()) {
                    addCorsHeaders(response, "*");
                } else {
                    // 简化处理，只添加第一个允许的Origin
                    addCorsHeaders(response, config_.allowedOrigins.front());
                }
            }
        }

        bool CorsMiddleware::isOriginAllowed(const std::string& origin) const {
            return config_.allowedOrigins.empty() ||
                std::find(config_.allowedOrigins.begin(), config_.allowedOrigins.end(), "*") != config_.allowedOrigins.end() ||
                std::find(config_.allowedOrigins.begin(), config_.allowedOrigins.end(), origin) != config_.allowedOrigins.end();
        }

        void CorsMiddleware::handlePreflightRequest(const HttpRequest& request, HttpResponse& response) {
            const std::string& origin = request.getHeader("Origin");
            if (!isOriginAllowed(origin)) {
                LOG_WARN << "CORS origin not allowed: " << origin;
                response.setStatusCode(HttpResponse::StatusCode::k403Forbidden); 
                return;
            }

            addCorsHeaders(response, origin);
            response.setStatusCode(HttpResponse::StatusCode::k204NoContent); 
            LOG_INFO << "CORS preflight request handled successfully";
        }

        void CorsMiddleware::addCorsHeaders(HttpResponse& response, const std::string& origin) const {
            try {
                response.addHeader("Access-Control-Allow-Origin", origin);

                if (config_.allowCredentials) {
                    response.addHeader("Access-Control-Allow-Credentials", "true");
                }

                if (!config_.allowedMethods.empty()) {
                    response.addHeader("Access-Control-Allow-Methods", join(config_.allowedMethods, ", "));
                }

                if (!config_.allowedHeaders.empty()) {
                    response.addHeader("Access-Control-Allow-Headers", join(config_.allowedHeaders, ", "));
                }

                response.addHeader("Access-Control-Max-Age", std::to_string(config_.maxAge));

                LOG_DEBUG << "CORS headers added to response";
            }
            catch (const std::exception& e) {
                LOG_ERROR << "Error adding CORS headers: " << e.what();
            }
        }

        std::string CorsMiddleware::join(const std::vector<std::string>& strings, const std::string& delimiter) const {
            std::ostringstream oss;
            for (size_t i = 0; i < strings.size(); ++i) {
                if (i != 0) {
                    oss << delimiter;
                }
                oss << strings[i];
            }
            return oss.str();
        }
    }
}

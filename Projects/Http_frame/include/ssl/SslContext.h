#pragma once

#include <openssl/ssl.h>
#include <muduo/base/noncopyable.h>
#include <memory>

#include "ssl/SslConfig.h"

namespace ssl {

    class SslContext : public muduo::noncopyable {
    public:
        explicit SslContext(const SslConfig& config);
        ~SslContext();

        bool initialize();
        SSL_CTX* getNativeHandle() const { return ctx_; }

    private:
        bool loadCertificates();
        bool setupProtocols();
        bool setupSessionCache();
        static void handleOpenSSLErrors(const char* msg);

    private:
        SSL_CTX*  ctx_;     // SSL context pointer
        SslConfig config_;  // SSL configuration
    };
}
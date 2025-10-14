#include "SslContext.h"
#include <openssl/err.h>
#include <muduo/base/Logging.h>

namespace ssl {

    SslContext::SslContext(const SslConfig& config)
        : ctx_(nullptr), config_(config) {
    }

    SslContext::~SslContext() {
        if (ctx_) {
            SSL_CTX_free(ctx_);
        }
    }

    bool SslContext::initialize() {
        // initialize OPENSSL
        OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, nullptr);

        // create SSL context
        const SSL_METHOD* method = TLS_server_method();
        ctx_ = SSL_CTX_new(method);
        if (!ctx_) {
            handleOpenSSLErrors("Unable to create SSL context");
            return false;
        }

        // set SSL options
        long options = SSL_OP_NO_SSLv2
            | SSL_OP_NO_SSLv3
            | SSL_OP_NO_COMPRESSION
            | SSL_OP_CIPHER_SERVER_PREFERENCE;
        SSL_CTX_set_options(ctx_, options);

        SSL_CTX_set_info_callback(ctx_, [](const SSL* s, int where, int ret) {
            if (where & SSL_CB_HANDSHAKE_START) LOG_INFO << "Handshake start\n";
            if (where & SSL_CB_HANDSHAKE_DONE) LOG_INFO << "Handshake done\n";
            });


        // load certificates
        if (!loadCertificates()) return false;

        // setup protocols
        if (!setupProtocols()) return false;

        // setup session cache
        if (!setupSessionCache()) return false;

        LOG_INFO << "SSL context initialized successfully";
        return true;
    }

    bool SslContext::loadCertificates() {
        // Load certificate
        if (SSL_CTX_use_certificate_file(ctx_, config_.getCertFile().c_str(), SSL_FILETYPE_PEM) <= 0) {
            handleOpenSSLErrors("Failed to load certificate file");
            return false;
        }

        // Load private key
        if (SSL_CTX_use_PrivateKey_file(ctx_, config_.getKeyFile().c_str(), SSL_FILETYPE_PEM) <= 0) {
            handleOpenSSLErrors("Failed to load private key file");
            return false;
        }

        // Verify private key
        if (!SSL_CTX_check_private_key(ctx_)) {
            LOG_ERROR << "Private key does not match the public certificate";
            return false;
        }

        // Load certificate chain if provided
        if (!config_.getCertificateChainFile().empty()) {
            if (SSL_CTX_use_certificate_chain_file(ctx_, config_.getCertificateChainFile().c_str()) <= 0) {
                handleOpenSSLErrors("Failed to load certificate chain file");
                return false;
            }
        }

        LOG_INFO << "Certificates loaded successfully";
        return true;
    }

    bool SslContext::setupProtocols() {
        // Set SSL/TLS protocol versions
        switch (config_.getSSLVersion())
        {
        case SSLVersion::TLSv1_0:
            SSL_CTX_set_min_proto_version(ctx_, TLS1_VERSION);
            break;
        case SSLVersion::TLSv1_1:
            SSL_CTX_set_min_proto_version(ctx_, TLS1_1_VERSION);
            break;
        case SSLVersion::TLSv1_2:
            SSL_CTX_set_min_proto_version(ctx_, TLS1_2_VERSION);
            break;
        case SSLVersion::TLSv1_3:
            SSL_CTX_set_min_proto_version(ctx_, TLS1_3_VERSION);
            break;
        default:
            LOG_ERROR << "Unsupported SSL/TLS version";
            return false;
        }

        // Set cipher list
        if (!config_.getCipherList().empty()) {
            if (SSL_CTX_set_cipher_list(ctx_, config_.getCipherList().c_str()) <= 0) {
                handleOpenSSLErrors("Failed to set cipher list");
                return false;
            }
        }

        LOG_INFO << "SSL protocols and ciphers configured successfully";
        return true;
    }

    bool SslContext::setupSessionCache() {
        SSL_CTX_set_session_cache_mode(ctx_, SSL_SESS_CACHE_SERVER);
        SSL_CTX_set_timeout(ctx_, config_.getSessionTimeout());
        SSL_CTX_sess_set_cache_size(ctx_, config_.getSessionCacheSize());
        return true;
    }

    void SslContext::handleOpenSSLErrors(const char* msg) {
        char buf[256];
        ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
        LOG_ERROR << msg << ": " << buf;
    }

}
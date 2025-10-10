#pragma once

#include <string>

#include "SslTypes.h"

namespace ssl {
    class SslConfig {
    public:
        SslConfig()
            : version_(SSLVersion::TLSv1_2)
            , cipherList_("HIGH:!aNULL:!MD5")
            , verifyPeer_(false)
            , verifyDepth_(4)
            , sessionTimeout_(300)
            , sessionCacheSize_(20480L) {}
        ~SslConfig() = default;

        // Setters
        void setCertFile(const std::string& certFile) { certFile_ = certFile; }
        void setKeyFile(const std::string& keyFile) { keyFile_ = keyFile; }
        void setCertificateChainFile(const std::string& chainFile) { chainFile_ = chainFile; }
        void setSSLVersion(SSLVersion version) { version_ = version; }
        void setCipherList(const std::string& cipherList) { cipherList_ = cipherList; }
        void setVerifyPeer(bool verify) { verifyPeer_ = verify; }
        void setVerifyDepth(int depth) { verifyDepth_ = depth; }
        void setSessionTimeout(int timeout) { sessionTimeout_ = timeout; }
        void setSessionCacheSize(long size) { sessionCacheSize_ = size; }

        // Getters
        const std::string& getCertFile() const { return certFile_; }
        const std::string& getKeyFile() const { return keyFile_; }
        const std::string& getCertificateChainFile() const { return chainFile_; }
        SSLVersion getSSLVersion() const { return version_; }
        const std::string& getCipherList() const { return cipherList_; }
        bool getVerifyPeer() const { return verifyPeer_; }
        int getVerifyDepth() const { return verifyDepth_; }
        int getSessionTimeout() const { return sessionTimeout_; }
        long getSessionCacheSize() const { return sessionCacheSize_; }  

    private:
        std::string certFile_;          // Path to the SSL certificate file
        std::string keyFile_;           // Path to the SSL private key file
        std::string chainFile_;         // Path to the SSL certificate chain file
        SSLVersion  version_;           // SSL/TLS version to use
        std::string cipherList_;        // List of allowed ciphers
        bool        verifyPeer_;        // Whether to verify the peer's certificate
        int         verifyDepth_;       // Maximum depth for certificate verification
        int         sessionTimeout_;    // SSL session timeout in seconds
        long        sessionCacheSize_;  // Size of the SSL session cache
    };
}
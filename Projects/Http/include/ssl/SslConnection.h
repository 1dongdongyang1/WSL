#pragma once

#include "SslContext.h"
#include <openssl/ssl.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/noncopyable.h>
#include <memory>

namespace ssl {

    using MessageCallback = std::function<void(const muduo::net::TcpConnectionPtr&,
        muduo::net::Buffer*, muduo::Timestamp)>;

    /**
     * @brief SSL Connection wrapper around a TCP connection
     */
    class SslConnection : public muduo::noncopyable {
    public:
        using TcpConnectionPtr = std::shared_ptr<muduo::net::TcpConnection>;
        using BufferPtr = muduo::net::Buffer*;

        SslConnection(const TcpConnectionPtr& conn, SslContext* ctx);
        ~SslConnection();

        void startHandshake();
        void send(const void* data, size_t len);
        void onRead(const TcpConnectionPtr& conn, BufferPtr buf, muduo::Timestamp receiveTime);

        // SSL BIO operations
        static int bioRead(BIO* bio, char* buf, int len);
        static int bioWrite(BIO* bio, const char* buf, int len);
        static long bioCtrl(BIO* bio, int cmd, long num, void* ptr);

        // Getters
        bool isHandshakeComplete() const { return state_ == SSLState::CONNECTED; }
        BufferPtr getDecryptedBuffer() { return &decryptedBuffer_; }

        // Setters
        void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

    private:
        void handleHandshake();
        void onEncryptedData(const char* data, size_t len);
        void onDecryptedData(const char* data, size_t len);
        SSLError getSSLError(int ret);
        void handleSSLError(SSLError error);

    private:
        muduo::net::Buffer readBuffer_;   
        muduo::net::Buffer writeBuffer_;
        muduo::net::Buffer decryptedBuffer_;    // Buffer for decrypted data
        MessageCallback    messageCallback_;
        SSL*               ssl_;                // SSL connection pointer
        SslContext*        ctx_;                // SSL context pointer
        TcpConnectionPtr   conn_;               // Underlying TCP connection
        SSLState           state_;              // Current SSL state
        BIO*               readBio_;            // BIO for reading
        BIO*               writeBio_;           // BIO for writing
    };
}
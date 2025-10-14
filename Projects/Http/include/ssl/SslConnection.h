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

        SslConnection(const TcpConnectionPtr& conn, SslContext* ctx);
        ~SslConnection();

        void startHandshake();
        void send(const std::string& data);
        void onTcpRead(const TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp receiveTime);

        // Getters && Setters
        bool isHandshakeComplete() const { return state_ == SSLState::CONNECTED; }
        void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
        void setDecryptedCallback(const MessageCallback& cb) { decryptedCallback_ = cb; }

    private:
        void doHandshake();
        void flushEncryptedData();
        void readDecryptedData(muduo::Timestamp receiveTime);
        void shutdown();

        SSLError getSSLError(int ret);
        void handleSSLError(SSLError error);

    private:
        TcpConnectionPtr   conn_;

        SSL*               ssl_;                // SSL connection pointer
        SslContext*        ctx_;                // SSL context pointer
        SSLState           state_;              // Current SSL state

        // Channel + Buffer
        BIO* readBio_;
        BIO* writeBio_;

        MessageCallback    messageCallback_;
        MessageCallback    decryptedCallback_;
    };
}
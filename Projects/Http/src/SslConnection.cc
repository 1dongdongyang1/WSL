#include "SslConnection.h"

#include <openssl/err.h>
#include <muduo/base/Logging.h>

namespace ssl {

    SslConnection::SslConnection(const TcpConnectionPtr& conn, SslContext* ctx)
        : conn_(conn)
        , ctx_(ctx)
        , ssl_(nullptr)
        , state_(SSLState::HANDSHAKE)
        , readBio_(nullptr)
        , writeBio_(nullptr)
        , messageCallback_(nullptr) {

        // create SSL object
        ssl_ = SSL_new(ctx_->getNativeHandle());
        if (!ssl_) {
            LOG_ERROR << "Failed to create SSL object";
            return;
        }

        readBio_ = BIO_new(BIO_s_mem());
        writeBio_ = BIO_new(BIO_s_mem());
        if (!readBio_ || !writeBio_) {
            LOG_ERROR << "Failed to create BIOs";
            SSL_free(ssl_);
            ssl_ = nullptr;
            return;
        }

        // 让SSL使用自定义的BIO
        SSL_set_bio(ssl_, readBio_, writeBio_);
        SSL_set_accept_state(ssl_);

        // Set TCP connection callbacks
        conn_->setMessageCallback(std::bind(&SslConnection::onTcpRead, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    SslConnection::~SslConnection() {
        if (ssl_) {
            // SSL_free will also free the BIOs
            SSL_free(ssl_);
        }
    }

    void SslConnection::startHandshake() {
        if (state_ != SSLState::HANDSHAKE) {
            LOG_WARN << "SSL handshake already in progress or completed";
            return;
        }
        doHandshake();
    }

    void SslConnection::send(const std::string& data) {
        if (state_ != SSLState::CONNECTED) {
            LOG_WARN << "SSL connection not established. Cannot send data.";
            return;
        }

        // 明文写入SSL后，SSL会加密数据并写入writeBio_
        int ret = SSL_write(ssl_, data.data(), static_cast<int>(data.size()));
        if (ret <= 0) {
            SSLError error = getSSLError(ret);
            handleSSLError(error);
            shutdown();
            return;
        }

        // Flush encrypted data to the TCP connection
        flushEncryptedData();
    }

    void SslConnection::flushEncryptedData() {
        if (state_ == SSLState::CLOSED) {
            return;
        }

        char buf[4096];
        int bytesRead = 0;
        while ((bytesRead = BIO_read(writeBio_, buf, sizeof(buf))) > 0) {
            conn_->send(buf, static_cast<size_t>(bytesRead));   // Send encrypted data over TCP
        }
    }

    // Handle incoming data from the TCP connection
    void SslConnection::onTcpRead(const TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp receiveTime) {
        BIO_write(readBio_, buf->peek(), buf->readableBytes());
        buf->retrieveAll();

        if(state_ == SSLState::HANDSHAKE) {
            doHandshake();
            return;
        }

        if(state_ != SSLState::CONNECTED) {
            return; // Handshake not complete yet
        }

        readDecryptedData(receiveTime);
    }

    // Read decrypted data from SSL and pass it to the message callback
    void SslConnection::readDecryptedData(muduo::Timestamp receiveTime) {
        if (state_ != SSLState::CONNECTED) {
            return;
        }

        char buf[4096];
        for (;;) {
            int ret = SSL_read(ssl_, buf, sizeof(buf));
            if( ret > 0 ) {
                if (decryptedCallback_) {
                    muduo::net::Buffer decryptedBuf;
                    decryptedBuf.append(buf, static_cast<size_t>(ret));
                    decryptedCallback_(conn_, &decryptedBuf, receiveTime);
                    decryptedBuf.retrieveAll();
                }
            }
            else {
                SSLError error = getSSLError(ret);
                if (error == SSLError::WANT_READ || error == SSLError::WANT_WRITE) {
                    break; // No more data to read
                }
                else {
                    handleSSLError(error);
                    shutdown();
                    break;
                }
            }
        }

        // Flush any pending encrypted data
        flushEncryptedData();
    }

    void SslConnection::doHandshake() {
        if (state_ != SSLState::HANDSHAKE) {
            return;
        }

        int ret = SSL_do_handshake(ssl_);
        if (ret == 1) {
            state_ = SSLState::CONNECTED;
            LOG_INFO << "SSL handshake completed successfully";
            LOG_INFO << "Using cipher: " << SSL_get_cipher(ssl_);
            LOG_INFO << "Protocol version: " << SSL_get_version(ssl_);

            flushEncryptedData();
            return;
        }

        int err = SSL_get_error(ssl_, ret);
        if(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
            // Need more data, just flush any pending encrypted data
            flushEncryptedData();
            return;
        }

        LOG_ERROR << "SSL handshake failed: " << ERR_reason_error_string(ERR_get_error());
        shutdown();
    }

    void SslConnection::shutdown() {
        if (state_ == SSLState::CLOSED) {
            return;
        }

        SSL_shutdown(ssl_);
        flushEncryptedData();
        conn_->shutdown();
        state_ = SSLState::CLOSED;
    }

    SSLError SslConnection::getSSLError(int ret) {
        int err = SSL_get_error(ssl_, ret);
        switch (err) {
        case SSL_ERROR_NONE:
            return SSLError::NONE;
        case SSL_ERROR_WANT_READ:
            return SSLError::WANT_READ;
        case SSL_ERROR_WANT_WRITE:
            return SSLError::WANT_WRITE;
        case SSL_ERROR_SYSCALL:
            return SSLError::SYSCALL;
        case SSL_ERROR_SSL:
            return SSLError::SSL;
        default:
            return SSLError::UNKNOWN;
        }
    }

    void SslConnection::handleSSLError(SSLError error) {
        switch (error) {
        case SSLError::WANT_READ:
        case SSLError::WANT_WRITE:
            // Need to write more data
            break;
        case SSLError::SYSCALL:
        case SSLError::SSL:
        case SSLError::UNKNOWN:
            LOG_ERROR << "SSL error occurred, closing connection";
            conn_->shutdown();
            state_ = SSLState::CLOSED;
            break;
        default:
            break;
        }
    }

}
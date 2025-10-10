#include "SslConnection.h"

#include <openssl/err.h>
#include <muduo/base/Logging.h>

namespace ssl {

    // Create a custom BIO_METHOD for SSL BIO operations
    static BIO_METHOD* createBioMethod() {
        BIO_METHOD* method = BIO_meth_new(BIO_TYPE_MEM, "ssl_bio");
        BIO_meth_set_write(method, SslConnection::bioWrite);
        BIO_meth_set_read(method, SslConnection::bioRead);
        BIO_meth_set_ctrl(method, SslConnection::bioCtrl);
        return method;
    }

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

        BIO_METHOD* bioMethod = createBioMethod();
        readBio_ = BIO_new(bioMethod);
        writeBio_ = BIO_new(bioMethod);
        BIO_meth_free(bioMethod);

        if (!readBio_ || !writeBio_) {
            LOG_ERROR << "Failed to create BIOs";
            SSL_free(ssl_);
            ssl_ = nullptr;
            return;
        }

        SSL_set_bio(ssl_, readBio_, writeBio_);
        SSL_set_accept_state(ssl_); // Set to server mode

        // Enable partial writes
        SSL_set_mode(ssl_, SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

        // Set TCP connection callbacks
        conn_->setMessageCallback(std::bind(&SslConnection::onRead, this,
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
        handleHandshake();
    }

    void SslConnection::send(const void* data, size_t len) {
        if (state_ != SSLState::CONNECTED) {
            LOG_WARN << "SSL connection not established. Cannot send data.";
            return;
        }

        int ret = SSL_write(ssl_, data, static_cast<int>(len));
        if (ret <= 0) {
            SSLError error = getSSLError(ret);
            handleSSLError(error);
            return;
        }

        // Write encrypted data to the write BIO
        char buffer[4096];
        int pending;
        while((pending = BIO_pending(writeBio_)) > 0) {
            int bytesRead = BIO_read(writeBio_, buffer, std::min(pending, static_cast<int>(sizeof(buffer))));
            if (bytesRead > 0) {
                conn_->send(buffer, bytesRead);
            } else if (bytesRead < 0) {
                LOG_ERROR << "BIO_read failed during send";
                break;
            }
        }
    }

    // Handle incoming data from the TCP connection
    void SslConnection::onRead(const TcpConnectionPtr& conn, BufferPtr buf, muduo::Timestamp receiveTime) {
        if (state_ == SSLState::HANDSHAKE) {
            // During handshake, buffer data and attempt to complete handshake
            BIO_write(readBio_, buf->peek(), buf->readableBytes());
            buf->retrieveAll();
            handleHandshake();
            return;
        }
        else if(state_ == SSLState::CONNECTED) {
            // encrypted data
            char decrypted[4096];
            int ret = SSL_read(ssl_, decrypted, sizeof(decrypted));
            if (ret > 0) {
                // create new buffer for decrypted data
                muduo::net::Buffer decryptedBuf;
                decryptedBuf.append(decrypted, ret);

                // callback with decrypted data
                if (messageCallback_) {
                    messageCallback_(conn_, &decryptedBuf, receiveTime);
                }
            }
        }
    }

    void SslConnection::handleHandshake() {
        if (state_ != SSLState::HANDSHAKE) {
            return;
        }

        int ret = SSL_do_handshake(ssl_);
        if (ret == 1) {
            state_ = SSLState::CONNECTED;
            LOG_INFO << "SSL handshake completed successfully";
            LOG_INFO << "Using cipher: " << SSL_get_cipher(ssl_);
            LOG_INFO << "Protocol version: " << SSL_get_version(ssl_);

            if(!messageCallback_) {
                LOG_WARN << "No message callback set for SslConnection";
                return;
            }
        }

        int err = SSL_get_error(ssl_, ret);
        switch (err)
        {
        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
            break;
        default: {
            char errBuf[256];
            unsigned long errCode = ERR_get_error();
            ERR_error_string_n(errCode, errBuf, sizeof(errBuf));
            LOG_ERROR << "SSL handshake failed: " << errBuf;
            conn_->shutdown();
            state_ = SSLState::CLOSED;
            break;
        }
        }
    }

    // TODO
    void SslConnection::onEncryptedData(const char* data, size_t len) {
        writeBuffer_.append(data, len);
        conn_->send(&writeBuffer_);
        writeBuffer_.retrieveAll();
    }

    void SslConnection::onDecryptedData(const char* data, size_t len) {
        decryptedBuffer_.append(data, len);
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

    int SslConnection::bioRead(BIO* bio, char* buf, int len) {
        if (buf == nullptr || len <= 0) {
            return 0;
        }

        SslConnection* sslConn = static_cast<SslConnection*>(BIO_get_data(bio));
        if (!sslConn) return -1;

        int bytesRead = sslConn->readBuffer_.readableBytes();
        if (bytesRead > 0) {
            int toRead = std::min(len, bytesRead);
            memcpy(buf, sslConn->readBuffer_.peek(), toRead);
            sslConn->readBuffer_.retrieve(toRead);
            return toRead;
        }
        return -1; // No data available
    }

    int SslConnection::bioWrite(BIO* bio, const char* buf, int len) {
        if (buf == nullptr || len <= 0) {
            return 0;
        }

        SslConnection* sslConn = static_cast<SslConnection*>(BIO_get_data(bio));
        if (!sslConn) return -1;

        sslConn->onEncryptedData(buf, len);
        return len;
    }

    long SslConnection::bioCtrl(BIO* bio, int cmd, long num, void* ptr) {
        switch (cmd) {
        case BIO_CTRL_FLUSH:
            return 1; // Always successful
        default:
            return 0;
        }
    }
}
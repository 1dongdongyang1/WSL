#pragma once

#include <string>

namespace ssl {
    enum class SSLVersion {
        TLSv1_0,
        TLSv1_1,
        TLSv1_2,
        TLSv1_3
    };

    enum class SSLError {
        NONE,
        WANT_READ,      // Non-blocking read operation did not complete
        WANT_WRITE,     // Non-blocking write operation did not complete
        SYSCALL,        // I/O error occurred
        SSL,            // A failure in the SSL library occurred
        UNKNOWN
    };

    enum class SSLState {
        NONE,
        HANDSHAKE,  // In the process of SSL handshake
        CONNECTED,  // SSL connection established
        CLOSED
    };
}
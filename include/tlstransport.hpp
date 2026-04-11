#pragma once 

#include "itransport.hpp"
#include "tcpsocket.hpp"
#include <openssl/ssl.h>
#include <openssl/err.h>

/**
 * TLSTransport
 *
 * Transport implementation providing TLS over a TCP connection using OpenSSL.
 *
 * Responsibilities:
 *  - perform TLS handshake (non-blocking)
 *  - encrypt outgoing data and decrypt incoming data
 *  - integrate with multiplexer via readiness checks
 *
 * State model:
 *  - INIT: socket assigned, handshake not started
 *  - HANDSHAKING: handshake in progress (non-blocking)
 *  - READY: handshake complete, secure communication established
 *  - ERROR/CLOSED: connection terminated or failed
 *
 * Notes:
 *  - handshake progresses incrementally via is_ready()
 *  - is_ready() has side effects (advances handshake state)
 *  - requires repeated calls until handshake completes
 *
 * TLS Context:
 *  - uses shared static SSL_CTX for client and server modes
 *  - contexts are initialised once per process
 *
 * Limitations:
 *  - certificate verification is manual (not full PKI validation)
 *  - no configurable certificate paths (hardcoded)
 *  - not thread-safe due to shared static context
 */
class TLSTransport : public ITransport {
private:
    TCPSocket socket_;
    static SSL_CTX *server_ctx_;
    static SSL_CTX *client_ctx_;
    SSL *ssl_;
    bool server_;
    bool handshake_complete_;

public:
    /**
     * @param socket Connected TCP socket
     * @param server True for server mode, false for client mode
     */
    explicit TLSTransport(TCPSocket, bool);

     /**
     * Cleans up TLS state and underlying SSL object
     */
    ~TLSTransport();

    /**
     * Returns underlying file descriptor
     */
    int fd() const override;

    /**
     * Receives decrypted data from TLS connection
     *
     * @return >0 bytes read
     * @return 0 connection closed
     * @return -1 retry required (non-blocking)
     */
    ssize_t recieve(uint8_t *, size_t) override;

    /**
     * Sends encrypted data over TLS connection
     *
     * Retries internally until buffer is fully sent or fails
     *
     * @return true on success
     * @return false on connection closure
     */
    bool send_all(const uint8_t *, size_t) override;

    /**
     * Advances TLS handshake state
     *
     * Must be called repeatedly until complete
     *
     * @return true if handshake complete
     * @return false if still in progress
     */
    bool do_handshake();

    /**
     * Indicates readiness for application-level I/O
     *
     * Side effects:
     *  - progresses handshake if not complete
     *
     * @return true if TLS session is ready
     */
    bool is_ready() override;

    /**
     * Verifies server certificate against provided PEM
     *
     * Note:
     *  - performs direct certificate comparison
     *  - does not use full certificate chain validation
     *
     * @throws std::runtime_error on mismatch or parsing failure
     */
    void verify_server_cert(const std::string &);
};

#pragma once 

#include "itransport.hpp"
#include "tcpsocket.hpp"
#include <openssl/ssl.h>
#include <openssl/err.h>

class TLSTransport : public ITransport {
private:
    TCPSocket socket_;
    static SSL_CTX *server_ctx_;
    static SSL_CTX *client_ctx_;
    SSL *ssl_;
    bool server_;
    bool handshake_complete_;

public:
    explicit TLSTransport(TCPSocket, bool);
    ~TLSTransport();

    int fd() const override;
    ssize_t recieve(uint8_t *, size_t) override;
    bool send_all(const uint8_t *, size_t) override;
    bool do_handshake();
};

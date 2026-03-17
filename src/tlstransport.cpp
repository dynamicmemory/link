// TODO: Add configurable path for certs so user can provide there own.
// TODO: Handle points on shutdown

#include "tlstransport.hpp"
#include <stdexcept>
#include <fcntl.h>
#include <iostream>

SSL_CTX *TLSTransport::server_ctx_ = nullptr;
SSL_CTX *TLSTransport::client_ctx_ = nullptr;

/**/
TLSTransport::TLSTransport(TCPSocket socket, bool server) : 
    socket_(std::move(socket)), server_(server) {

    if (!server_ctx_ && !client_ctx_) {
        SSL_library_init();
        OpenSSL_add_ssl_algorithms();
        SSL_load_error_strings();

        server_ctx_ = ::SSL_CTX_new(TLS_server_method());
        client_ctx_ = ::SSL_CTX_new(TLS_client_method());

        ::SSL_CTX_use_certificate_file(server_ctx_, "../certs/server.crt", SSL_FILETYPE_PEM);
        ::SSL_CTX_use_PrivateKey_file(server_ctx_, "../certs/server.key", SSL_FILETYPE_PEM);
    }

    SSL_CTX *ctx = server_ ? server_ctx_: client_ctx_;
    ssl_ = ::SSL_new(ctx);

    // TODO: Add non_blocking() in tcpsocket to flip on and off. 
    int flags = ::fcntl(socket_.fd(), F_GETFL, 0);
    fcntl(socket_.fd(), F_SETFL, flags | O_NONBLOCK);
    SSL_set_fd(ssl_, socket_.fd()); 

    handshake_complete_ = false;
};

TLSTransport::~TLSTransport() {
    if (ssl_) {
        ::SSL_shutdown(ssl_);
        ::SSL_free(ssl_);
        ssl_ = nullptr;
    }
}

/**/
int TLSTransport::fd() const { return socket_.fd();}

/**/
bool TLSTransport::send_all(const uint8_t *data, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t n = ::SSL_write(ssl_, data+total, len-total);
        if (n > 0) {
            total += n;
            continue;
        }
        // Didnt read anything but isnt complete yet
        int err = ::SSL_get_error(ssl_, n);
        if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
            continue;
        }
        // The return type is bool, does this even make sense?
        if (err == SSL_ERROR_ZERO_RETURN) {  
            handshake_complete_ = false;
            return false;
        }
        // Catastrophic error occured TODO: Handle better.
        throw std::runtime_error("SSL Write failed");
    }
    return true;
}

/**/
ssize_t TLSTransport::recieve(uint8_t *buf, size_t n) { 
    ssize_t ret = ::SSL_read(ssl_, buf, n);
    if (ret > 0) return ret;

    int err = ::SSL_get_error(ssl_, ret);
    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) return -1;
    // Simplify this
    if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL || ret == 0) {
        handshake_complete_ = false;
        return 0;
    }

    // Catastrophic error occured TODO: Handle better.
    std::cout << "ret val = " << ret << '\n';
    throw std::runtime_error("SSL Read failed");
}

// TODO: Can return void if new solution works
bool TLSTransport::do_handshake() {
    int ret = (server_) ? ::SSL_accept(ssl_) : ::SSL_connect(ssl_); 

    if (ret == 1) {
        handshake_complete_ = true;
        return true;
    }

    int err = ::SSL_get_error(ssl_, ret);
    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
        handshake_complete_ = false;
        return false;
    }
    if (err == SSL_ERROR_ZERO_RETURN) {
        handshake_complete_ = false;
        return 0;
    }

    // Catastrophic failure TODO: Better handling of this failure.
    char buf[256];
    ::ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
    throw std::runtime_error(std::string("TLS Handshake failed.")+buf);
}

bool TLSTransport::is_ready() {
    if (handshake_complete_)
        return handshake_complete_;
    do_handshake();
    return handshake_complete_;
}

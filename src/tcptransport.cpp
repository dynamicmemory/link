#include "tcptransport.hpp"

/**/
TCPTransport::TCPTransport(TCPSocket socket) : socket_(std::move(socket)) {};

int TCPTransport::fd() const { return socket_.fd();}

/*
 * Delegates full-buffer send to underlying socket
 */
bool TCPTransport::send_all(const uint8_t *data, size_t len) {
    return socket_.send_all(data, len);
}

/*
 * Thin wrapper over recv()
 */
ssize_t TCPTransport::recieve(uint8_t *buf, size_t n) { 
    return socket_.recieve(buf, n); 
}

/*
 * TCP has no handshake phase → always ready
 */
bool TCPTransport::is_ready() {
    return true;
}

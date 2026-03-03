#include "tcptransport.hpp"

/**/
TCPTransport::TCPTransport(TCPSocket socket) : socket_(std::move(socket)) {};

/**/
int TCPTransport::fd() const { return socket_.fd();}

/**/
bool TCPTransport::send_all(const uint8_t *data, size_t len) {
    return socket_.send_all(data, len);
}

/**/
ssize_t TCPTransport::recieve(uint8_t *buf) { return socket_.recieve(buf); }


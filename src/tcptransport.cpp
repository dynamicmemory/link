/* The TCPTransport class implements the ITransport interface and delegates all
 * network operations to an underlying TCPSocket instance. */
#include "tcptransport.hpp"

/**/
TCPTransport::TCPTransport(TCPSocket socket) : socket_(std::move(socket)) {};

/* Returns the sockets fd*/
int TCPTransport::fd() const { return socket_.fd();}

/* Sends a complete buffer to the underlying socket*/
bool TCPTransport::send_all(const uint8_t *data, size_t len) {
    return socket_.send_all(data, len);
}

/* Receives data from the TCP connection.
 * @return 0 if the remote peer closed the connection, -1 on error*/
ssize_t TCPTransport::recieve(uint8_t *buf, size_t n) { 
    return socket_.recieve(buf, n); 
}

/* Returns true if the underlying socket is ready for communication  
 * Note: There is no handshake for raw tcp, so always return true */
bool TCPTransport::is_ready() {
    return true;
}

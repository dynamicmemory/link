#include "tcptransport.hpp"

/**/
TCPTransport::TCPTransport(TCPSocket socket) : socket_(std::move(socket)) {};

/* @return The POSIX file descriptor associated with this transport. */
int TCPTransport::fd() const { return socket_.fd();}

/* Sends a complete buffer over the TCP connection.
 * @param data Pointer to the buffer containing bytes to send.
 * @param len Number of bytes to transmit.
 *
 * @return true if the entire buffer was transmitted, false otherwise */
bool TCPTransport::send_all(const uint8_t *data, size_t len) {
    return socket_.send_all(data, len);
}

/* Receives data from the TCP connection.
 * @param buf Destination buffer for received bytes.
 *
 * @return 0 if the remote peer closed the connection, -1 on error*/
ssize_t TCPTransport::recieve(uint8_t *buf, size_t n) { 
    return socket_.recieve(buf, n); 
}


/* TCPTransport
 *
 * Concrete transport implementation that provides reliable byte-stream
 * communication over a TCP socket.
 *
 * This class implements the ITransport interface and delegates all
 * network operations to an underlying TCPSocket instance.
 */

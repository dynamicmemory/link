#pragma once 

#include "itransport.hpp"
#include "tcpsocket.hpp"

/**
 * TCPTransport
 *
 * Concrete transport implementation using plain TCP sockets.
 *
 * Responsibilities:
 *  - provide a uniform transport interface over a TCPSocket
 *  - handle raw byte transmission (send/receive)
 *
 * Characteristics:
 *  - connection-oriented, reliable byte stream
 *  - no message boundaries (handled by protocol layer)
 *  - no handshake phase (immediately ready after connection)
 *
 * Notes:
 *  - does not perform encryption or authentication
 *  - higher-level framing is delegated to IProtocol implementations
 */
class TCPTransport : public ITransport {
private:
    TCPSocket socket_;
public:
    /**
     * Takes ownership of a connected TCP socket.
     */
    explicit TCPTransport(TCPSocket);

    /**
     * Returns the underlying file descriptor.
     */
    int fd() const override;

   /**
     * Receives raw bytes from the connection.
     *
     * @return number of bytes read
     * @return 0 if the remote peer closed the connection
     * @return -1 on error (e.g. EAGAIN for non-blocking sockets)
     */
    ssize_t recieve(uint8_t *, size_t) override;

    /**
     * Sends the entire buffer over the connection.
     *
     * @return true if all bytes were sent
     * @return false on failure
     */
    bool send_all(const uint8_t *, size_t) override;

    /**
     * Indicates whether the transport is ready for use.
     *
     * For TCP, this always returns true since there is no handshake phase.
     */
    bool is_ready() override;
};

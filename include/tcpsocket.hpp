#pragma once
#include <string>
#include <netdb.h>
#include <unistd.h>

/**
 * TCPSocket
 *
 * A move-only RAII wrapper around a POSIX TCP socket file descriptor.
 *
 * Responsibilities:
 *  - owns a socket file descriptor (fd)
 *  - ensures automatic closure via destructor
 *  - provides basic send/receive operations
 *
 * Ownership semantics:
 *  - copy is disabled
 *  - move transfers ownership and invalidates the source (fd = -1)
 *
 * Valid state:
 *  - fd_ >= 0 → valid, open socket
 *  - fd_ < 0  → invalid / moved-from
 */
class TCPSocket {
private:
    int fd_;

    /*
     * Resolves host/port into addrinfo structures.
     * passive = true → server binding (AI_PASSIVE)
     * passive = false → client connection
     */
    static addrinfo *address_(const std::string &, const std::string &, bool);

    /*
     * Creates a socket from addrinfo configuration.
     */
    static int socket_(addrinfo *);

public: 
    /**
     * Constructs a socket wrapper from an existing file descriptor.
     * Takes ownership of the descriptor.
     */
    explicit TCPSocket(int);

    /**
     * Closes the socket if it is valid.
     */
    ~TCPSocket();

    TCPSocket(const TCPSocket &) = delete; 
    TCPSocket &operator=(const TCPSocket &) = delete;

    /**
     * Move constructor transfers ownership of the file descriptor.
     * The source object becomes invalid (fd = -1).
     */
    TCPSocket(TCPSocket &&);

    /**
     * Move assignment transfers ownership, closing any existing descriptor.
     */
    TCPSocket &operator=(TCPSocket &&);

    /**
     * Creates a server socket bound to (host, port).
     *
     * The returned socket is:
     *  - bound
     *  - NOT yet listening (listen_socket() must be called)
     */
    static TCPSocket server_socket(const std::string &, const std::string &);

    /**
     * Creates and connects a client socket to (host, port).
     *
     * The returned socket is:
     *  - fully connected
     */
    static TCPSocket client_socket(const std::string &, const std::string &);

    /**
     * Marks the socket as a listening socket.
     */
    void listen_socket();

   /**
     * Accepts an incoming client connection on a listening socket.
     *
     * @param fd Listening socket file descriptor
     * @return New TCPSocket representing the client connection
     */
    static TCPSocket accept_client(int);

    /**
     * Sends the entire buffer over the socket.
     *
     * This function repeatedly calls send() until:
     *  - all bytes are transmitted → returns true
     *  - an unrecoverable error occurs → returns false
     *
     * EINTR is handled internally (retry).
     */
    bool send_all(const uint8_t *, size_t); 

    /**
     * Receives data from the socket.
     *
     * Return semantics:
     *  > 0 → number of bytes received
     *    0 → peer has closed the connection
     *  < 0 → error (EAGAIN/EWOULDBLOCK for non-blocking sockets)
     */
    ssize_t recieve(uint8_t *, size_t n);

    /**
     * Returns the underlying file descriptor.
     */
    int fd() const;
};



// TODO: Build logging system to log all // out print statements
/* TCPSocket manages the lifetime of a socket descriptor and ensures it is 
 * automatically close when the object is destroyed. 
 *
 * The class provides helper
 * functions for creating client and server sockets as well as all associated 
 * operations.
*/
#include <iostream>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include "tcpsocket.hpp"

// Constructors
TCPSocket::TCPSocket(int fd) : fd_(fd) {}
TCPSocket::~TCPSocket() { 
    if (fd_ >= 0) 
        ::close(fd_); 
    // std::cout << fd_ << " has been destoryed" << '\n'; 
}

// Moves
TCPSocket::TCPSocket(TCPSocket &&rhs) { 
    fd_ = rhs.fd_; 
    rhs.fd_ = -1;
}

TCPSocket &TCPSocket::operator=(TCPSocket &&rhs) {
    if (this == &rhs) return *this;
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = rhs.fd_;
        rhs.fd_ = -1;
    }
    return *this;
}

// Sets up addresses for bind and connect
addrinfo *TCPSocket::address_(const std::string &host, const std::string &port, bool passive) {
    struct addrinfo hints, *addr; 
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    if (passive) hints.ai_flags = AI_PASSIVE;
    // std::cout << "Setting up: " << host << ":" << port << '\n';

    int status = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
    if (status != 0) { 
        throw std::runtime_error("Could not resolve host/port (" +
                host + ":" + port + "): " + gai_strerror(status)); 
    }
    // std::cout << "getaddrinfo was successful" << '\n';

    return addr;
}

// Returns a socket
int TCPSocket::socket_(addrinfo *addr) {
    int fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (fd < 0) { throw std::runtime_error("Socket Failed"); }
    // std::cout << "socket was successful, fd: " << fd << '\n';

    return fd;
}

/* Creates and binds a TCP socket for server use, simplifies creation process.
 * Internally does: 
 *   - getaddrinfo()
 *   - socket()
 *   - bind()
 *
 * The returned socket is ready to enter the listening state.
 *
 * @param host Host address to bind to.
 * @param port Port number or service name.
 *
 * @return TCPSocket
*/
TCPSocket TCPSocket::server_socket(const std::string &host, const std::string &port) {
    struct addrinfo *addr = address_(host, port, true); 
    int fd = socket_(addr);

    int b = ::bind(fd, addr->ai_addr, addr->ai_addrlen);
    if (b != 0) {
        ::close(fd);
        freeaddrinfo(addr);
        throw std::runtime_error("Bind Failed");
    }
    // std::cout << "Bind was successful: " << b << '\n';
    freeaddrinfo(addr);

    return TCPSocket(fd);
}

/* Creates and connects a TCP socket for client use, simplifies creation process.
 * Internally does: 
 *   - getaddrinfo()
 *   - socket()
 *   - connect()
 *
 * The returned socket is ready to enter the listening state.
 *
 * @param host Server hostname or IP.
 * @param port Server Port number.
 *
 * @return TCPSocket
*/
TCPSocket TCPSocket::client_socket(const std::string &host, const std::string &port) {
    struct addrinfo *addr = address_(host, port, false); 
    int fd = socket_(addr);

    int c = ::connect(fd, addr->ai_addr, addr->ai_addrlen);
    if (c != 0) {
        ::close(fd);
        freeaddrinfo(addr);
        throw std::runtime_error("Connect Failed");
    }
    // std::cout << "connection was successful: " << c << '\n';
    freeaddrinfo(addr);

    return TCPSocket(fd);
}

/**/
void TCPSocket::listen_socket() {
    int listen = ::listen(fd_, SOMAXCONN);
    // std::cout << "Listen for connections " << listen << '\n';
}

/**/
TCPSocket TCPSocket::accept_client(int fd) {
    // std::cout << "Start of accept" << "\n";
    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_socket = ::accept(fd, (struct sockaddr *)&client_addr, 
            &addr_len);

    if (client_socket < 0) {
        throw std::runtime_error("Accept Failed");
    }
    // std::cout << "After accept call" << "\n";

    // std::cout << "Client accepted" << "\n";
    return TCPSocket(client_socket);
}

/**/
int TCPSocket::fd() const { return fd_; }

/* Sends the entire buffer over the socket.
 *
 * This function repeatedly calls send() until the entire
 * buffer has been transmitted or an unrecoverable error occurs.
 *
 * @param buf Pointer to data buffer.
 * @param len Number of bytes to send.
 *
 * @return true if all bytes were successfully sent.
 * @return false if a transmission error occurred.
 */
bool TCPSocket::send_all(const uint8_t *buf, size_t len) { 
    size_t total = 0;
    while (total < len) {
        ssize_t n = ::send(fd_, buf+total, len-total, 0);
        if (n <=0) {
            if (errno == EINTR) continue;
            return false;
        }
        total += n;
    }
    
    return true;
}

/* Receives data from the socket.
 *
 * @param buf Buffer to store received data.
 * @param len Maximum number of bytes to read.
 *
 * @return Number of bytes received.
 * @return 0 if the peer closed the connection.
 * @return -1 if an error occurred.
 */
ssize_t TCPSocket::recieve(uint8_t *buf, size_t n) { 
    return ::recv(fd_, buf, n, 0);
}

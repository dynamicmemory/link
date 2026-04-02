// TODO: Build logging system to log all // out print statements

/* TCPSocket manages the lifetime of a socket descriptor and ensures it is 
 * automatically closed when the object is destroyed. */
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

// Operator overloads
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
    if (fd < 0) 
        throw std::runtime_error(std::string("Socket Failed ") + 
                std::strerror(errno));
    // std::cout << "socket was successful, fd: " << fd << '\n';

    return fd;
}

/* Creates and binds a TCP socket for server use, the returned socket is ready 
 * to enter the listening state. */
TCPSocket TCPSocket::server_socket(const std::string &host, const std::string &port) {
    struct addrinfo *addr = address_(host, port, true); 
    int fd = socket_(addr);

    int b = ::bind(fd, addr->ai_addr, addr->ai_addrlen);
    if (b != 0) {
        ::close(fd);
        freeaddrinfo(addr);
        throw std::runtime_error(std::string("Error using socket - ") +
                std::strerror(errno));
    }
    // std::cout << "Bind was successful: " << b << '\n';
    freeaddrinfo(addr);

    return TCPSocket(fd);
}

/* Creates and connects a TCP socket for client use, The returned socket is 
 * ready to enter the listening state. */
TCPSocket TCPSocket::client_socket(const std::string &host, const std::string &port) {
    struct addrinfo *addr = address_(host, port, false); 
    int fd = socket_(addr);

    int c = ::connect(fd, addr->ai_addr, addr->ai_addrlen);
    if (c != 0) {
        ::close(fd);
        freeaddrinfo(addr);
        throw std::runtime_error(std::string("Error connecting socket - ") +
                std::strerror(errno));
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
    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_socket = ::accept(fd, (struct sockaddr *)&client_addr, 
            &addr_len);

    if (client_socket < 0) {
        throw std::runtime_error(std::string("Error accepting socket - ") +
                std::strerror(errno));
    }
    return TCPSocket(client_socket);
}

/* Returns the underlying fd for this socket */
int TCPSocket::fd() const { return fd_; }

/* Sends the entire buffer over the socket. This function repeatedly calls send() 
 * until the entire buffer has been transmitted or an unrecoverable error occurs.
 *
 * @return true if all bytes were successfully sent.
 * @return false if a transmission error occurred. */
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

/* Receives data from the socket. */
ssize_t TCPSocket::recieve(uint8_t *buf, size_t n) { 
    return ::recv(fd_, buf, n, 0);
}

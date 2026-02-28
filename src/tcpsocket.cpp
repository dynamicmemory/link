#include <iostream>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include "tcpsocket.hpp"

// Constructors
TCPSocket::TCPSocket(int fd) : fd_(fd) {}
TCPSocket::~TCPSocket() { if (fd_ >= 0) ::close(fd_); 
    std::cout << fd_ << " has been destoryed" << '\n'; }

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
    std::cout << "Setting up: " << host << ":" << port << '\n';

    int status = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
    if (status != 0) { throw std::runtime_error(gai_strerror(status)); }
    std::cout << "getaddrinfo was successful" << '\n';

    return addr;
}

// Returns a socket
int TCPSocket::socket_(addrinfo *addr) {
    int fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (fd < 0) { throw std::runtime_error("Socket Failed"); }
    std::cout << "socket was successful, fd: " << fd << '\n';

    return fd;
}

// Creates a socket for a server 
TCPSocket TCPSocket::server_socket(const std::string &host, const std::string &port) {
    struct addrinfo *addr = address_(host, port, true); 
    int fd = socket_(addr);

    int b = ::bind(fd, addr->ai_addr, addr->ai_addrlen);
    if (b != 0) {
        ::close(fd);
        freeaddrinfo(addr);
        throw std::runtime_error("Bind Failed");
    }
    std::cout << "Bind was successful: " << b << '\n';
    freeaddrinfo(addr);

    return TCPSocket(fd);
}

// Creates a socket for a client 
TCPSocket TCPSocket::client_socket(const std::string &host, const std::string &port) {
    struct addrinfo *addr = address_(host, port, false); 
    int fd = socket_(addr);

    int c = ::connect(fd, addr->ai_addr, addr->ai_addrlen);
    if (c != 0) {
        ::close(fd);
        freeaddrinfo(addr);
        throw std::runtime_error("Connect Failed");
    }
    std::cout << "connection was successful: " << c << '\n';
    freeaddrinfo(addr);

    return TCPSocket(fd);
}

/**/
void TCPSocket::listen_socket() {
    int listen = ::listen(fd_, SOMAXCONN);
    std::cout << "Listen for connections " << listen << '\n';
}

/**/
TCPSocket TCPSocket::accept_client(int fd) {
    std::cout << "Start of accept" << "\n";
    struct sockaddr client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_socket = ::accept(fd, (struct sockaddr *)&client_addr, 
            &addr_len);
    std::cout << "After accept call" << "\n";

    if (client_socket < 0) {
        throw std::runtime_error("Accept Failed");
    }

    std::cout << "Client accepted" << "\n";
    return TCPSocket(client_socket);
}

/**/
int TCPSocket::fd() { return fd_; }

/**/
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

/**/
ssize_t recieve_all() { /*Write send_all logic*/ return 0; }





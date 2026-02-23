#include <iostream>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include "tcpsocket.hpp"

TCPSocket::TCPSocket(int fd) : fd(fd) {}
TCPSocket::~TCPSocket() { if (fd >= 0) ::close(fd); }

TCPSocket::TCPSocket(TCPSocket &&rhs) { 
    fd = rhs.fd; 
    rhs.fd = -1;
}

TCPSocket &TCPSocket::operator=(TCPSocket &&rhs) {
    if (this == &rhs) return *this;
    if (fd >= 0) {
        ::close(fd);
        fd = rhs.fd;
        rhs.fd = -1;
    }
    return *this;
}

static addrinfo *address(const std::string &host, const std::string &port, bool passive) {
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

static int make_socket(addrinfo *addr) {
    int fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (fd < 0) { throw std::runtime_error("Socket Failed"); }
    std::cout << "socket was successful, fd: " << fd << '\n';

    return fd;
}

static TCPSocket server_socket(const std::string &host, const std::string &port) {
    struct addrinfo *addr = address(host, port, true); 
    int fd = make_socket(addr);

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

static TCPSocket client_socket(const std::string &host, const std::string &port) {
    struct addrinfo *addr = address(host, port, false); 
    int fd = make_socket(addr);

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
    int listen = ::listen(fd, SOMAXCONN);
    std::cout << "Listen for connections " << listen << '\n';
}

/**/
TCPSocket TCPSocket::accept_client() {
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

ssize_t send_all() { /*Write send_all logic*/ return 0; }
ssize_t recieve_all() { /*Write send_all logic*/ return 0; }

// int get_fd() { return fd; }



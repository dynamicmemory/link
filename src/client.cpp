#include "client.hpp"
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

Client::Client(const std::string &host, const std::string &port) : host(host), port(port) {}

void Client::create_client() {
    bool passive = false;
    struct addrinfo hints, *addr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    if (passive) hints.ai_flags = AI_PASSIVE;
    std::cout << "Setting up: " << host << ":" << port << '\n';

    int status = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
    if (status != 0) { throw std::runtime_error(gai_strerror(status)); }
    std::cout << "getaddrinfo was successful" << '\n';

    int fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (fd < 0) { throw std::runtime_error("Socket Failed"); }
    std::cout << "socket was successful, fd: " << fd << '\n';

    int c = ::connect(fd, addr->ai_addr, addr->ai_addrlen);
    if (c != 0) {
        ::close(fd);
        freeaddrinfo(addr);
        throw std::runtime_error("Connect Failed");
    }
    std::cout << "connection was successful: " << c << '\n';
    freeaddrinfo(addr);
}

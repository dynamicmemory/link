#include <iostream>
#include <string>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

class TCPSocket {
    int fd = -1;

public: 
    explicit TCPSocket(int fd) : fd(fd) {}
    ~TCPSocket() { if (fd >= 0) ::close(fd); }

    TCPSocket(const TCPSocket &) = delete;
    TCPSocket &operator=(const TCPSocket &) = delete;

    TCPSocket(TCPSocket &&rhs) { 
        fd = rhs.fd; 
        rhs.fd = -1;
    }

    static TCPSocket create_server(const std::string &host, const std::string &port) {
        struct addrinfo hints, *addr; 
        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        std::cout << "Setting up: " << host << ":" << port << '\n';

        int status = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
        if (status != 0) { throw std::runtime_error(gai_strerror(status)); }
        std::cout << "getaddrinfo was successful" << '\n';

        int fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (fd < 0) { throw std::runtime_error("Socket Failed"); }
        std::cout << "socket was successful, fd: " << fd << '\n';

        int b = ::bind(fd, addr->ai_addr, addr->ai_addrlen);
        if (b != 0) {
            throw std::runtime_error("Bind Failed");
            ::close(fd);
        }
        std::cout << "Bind was successful: " << b << '\n';
        freeaddrinfo(addr);

        return TCPSocket(fd);
    }

    static TCPSocket create_client(const std::string &host, const std::string &port) {
        struct addrinfo hints, *addr; 
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        std::cout << "Setting up: " << host << ":" << port << '\n';

        int status = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
        if (status != 0) { throw std::runtime_error(gai_strerror(status)); }
        std::cout << "getaddrinfo was successful" << '\n';

        int fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (fd < 0) { throw std::runtime_error("Socket Failed"); }
        std::cout << "socket was successful, fd: " << fd << '\n';

        int c = ::connect(fd, addr->ai_addr, addr->ai_addrlen);
        if (c != 0) {
            throw std::runtime_error("Connect Failed");
            ::close(fd);
        }
        std::cout << "connection was successful: " << c << '\n';
        freeaddrinfo(addr);

        return TCPSocket(fd);
    }

    /**/
    void listen() {
        int listen = ::listen(fd, SOMAXCONN);
        std::cout << "Listen was successful: " << listen << '\n';
    }

    /**/
    void accept() { /* Write accept logic*/ }

    ssize_t send_all() { /*Write send_all logic*/ }
    ssize_t recieve_all() { /*Write send_all logic*/ }
};


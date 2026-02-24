#include "server.hpp"
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

Server::Server(const std::string &host, const std::string &port) : host(host), port(port) {
    bool passive = true;
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

    // Lets server reuse address on quick restart, crash otherwise.
    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    int b = ::bind(fd, addr->ai_addr, addr->ai_addrlen);
    if (b != 0) {
        ::close(fd);
        freeaddrinfo(addr);
        throw std::runtime_error("Bind Failed");
    }
    std::cout << "Bind was successful: " << b << '\n';
    freeaddrinfo(addr);

    server_socket = fd;
}

void Server::listening() {
    int listen = ::listen(server_socket, SOMAXCONN);
    std::cout << "Listen for connections " << listen << '\n';
    if (listen < 0) {
        throw std::runtime_error("listen failed");
    }
}

void Server::start_server() {
    fd_set master;
    FD_ZERO(&master);
    FD_SET(server_socket, &master);
    int max_fd = server_socket;

    while (1) {
        fd_set fds = master;

        // struct timeval timeout;
        // timeout.tv_sec = 0;
        // timeout.tv_usec = 100000;

        if (select(max_fd+1, &fds, 0, 0, NULL) < 0) {
            throw std::runtime_error("select failed");
        }

        for (int fd=0; fd<=max_fd; ++fd) {
            if (!FD_ISSET(fd, &fds)) continue;

            // Client connecting to the server
            if (fd == server_socket) {
                std::cout << "New client attempting to connect" << "\n";

                // Accept Block
                struct sockaddr client_addr;
                socklen_t addr_len = sizeof(client_addr);
                int client_socket = ::accept(fd, (struct sockaddr *)&client_addr, 
                        &addr_len);
                std::cout << "After accept call" << "\n";

                if (client_socket < 0) {
                    throw std::runtime_error("Accept Failed");
                }

                // failed to connect, might not need check due to throw.
                if (client_socket < 0) continue; 
                FD_SET(client_socket, &master);
                if (client_socket > max_fd) max_fd = client_socket; 
                std::cout << "New client successfully connected" << "\n";
                // Accept Block end 
            }
            else {
                ssize_t total = 1024;
                char buf[total];
                ssize_t n = ::recv(fd,buf,total,0);

                // Client disconnecting 
                if (n == 0) {
                    close(fd);   // Not needed in socket abstraction
                    FD_CLR(fd, &master);
                    if (fd == max_fd)
                        while (max_fd >= 0 && !FD_ISSET(max_fd, &master))
                            max_fd--;
                    std::cerr << "Client has disconnected" << "\n";
                }

                // Error occured
                else if (n < 0) {
                    if (errno == EINTR) continue;
                    if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                }

                // Client sending a request
                else 
                std::cout << "Client sent: " << std::string(buf, n) << "\n";
            }
        }
    }
}


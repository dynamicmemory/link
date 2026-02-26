#include "server.hpp"
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

Server::Server(const std::string &host, const std::string &port) : 
    host(host), port(port), socket(TCPSocket::server_socket(host, port)) {}

void Server::listening() {
    int listen = ::listen(socket.fd(), SOMAXCONN);
    std::cout << "Listen for connections " << listen << '\n';
    if (listen < 0) {
        throw std::runtime_error("listen failed");
    }
}

void Server::start_server() {
    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket.fd(), &master);
    int max_fd = socket.fd();

    while (1) {
        fd_set fds = master;

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        if (select(max_fd+1, &fds, 0, 0, &timeout) < 0) {
            throw std::runtime_error("select failed");
        }

        for (int fd=0; fd<=max_fd; ++fd) {
            if (!FD_ISSET(fd, &fds)) continue;

            // Client connecting to the server
            if (fd == socket.fd()) {
                std::cout << "New client attempting to connect" << "\n";

                // Accept Block
                TCPSocket client_socket = TCPSocket::accept_client(fd);
                std::cout << "After accept call" << "\n";

                if (client_socket.fd() < 0) {
                    throw std::runtime_error("Accept Failed");
                }
                int cfd = client_socket.fd();
                connections.emplace(cfd, std::move(client_socket));

                // failed to connect, might not need check due to throw.
                if (cfd < 0) continue; 
                FD_SET(cfd, &master);
                if (cfd > max_fd) max_fd = cfd; 
                std::cout << "New client successfully connected" << "\n";
                // Accept Block end 
            }
            else {
                ssize_t total = 1024;
                char buf[total];
                ssize_t n = ::recv(fd,buf,total,0);

                // Client disconnecting 
                if (n == 0) {
                    // close(fd);   // Not needed in socket abstraction
                    FD_CLR(fd, &master);
                    connections.erase(fd);
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





// void Server::start_server() {
//     fd_set master;
//     FD_ZERO(&master);
//     FD_SET(socket.fd(), &master);
//     int max_fd = socket.fd();
//
//     while (1) {
//         fd_set fds = master;
//
//         struct timeval timeout;
//         timeout.tv_sec = 0;
//         timeout.tv_usec = 0;
//
//         if (select(max_fd+1, &fds, 0, 0, &timeout) < 0) {
//             throw std::runtime_error("select failed");
//         }
//
//         for (int fd=0; fd<=max_fd; ++fd) {
//             if (!FD_ISSET(fd, &fds)) continue;
//
//             // Client connecting to the server
//             if (fd == socket.fd()) {
//                 std::cout << "New client attempting to connect" << "\n";
//
//                 // Accept Block
//                 TCPSocket client_socket = TCPSocket::accept_client(fd);
//                 std::cout << "After accept call" << "\n";
//
//                 if (client_socket.fd() < 0) {
//                     throw std::runtime_error("Accept Failed");
//                 }
//                 int cfd = client_socket.fd();
//                 connections.emplace(cfd, std::move(client_socket));
//
//                 // failed to connect, might not need check due to throw.
//                 if (cfd < 0) continue; 
//                 FD_SET(cfd, &master);
//                 if (cfd > max_fd) max_fd = cfd; 
//                 std::cout << "New client successfully connected" << "\n";
//                 // Accept Block end 
//             }
//             else {
//                 ssize_t total = 1024;
//                 char buf[total];
//                 ssize_t n = ::recv(fd,buf,total,0);
//
//                 // Client disconnecting 
//                 if (n == 0) {
//                     // close(fd);   // Not needed in socket abstraction
//                     FD_CLR(fd, &master);
//                     connections.erase(fd);
//                     if (fd == max_fd)
//                         while (max_fd >= 0 && !FD_ISSET(max_fd, &master))
//                             max_fd--;
//                     std::cerr << "Client has disconnected" << "\n";
//                 }
//
//                 // Error occured
//                 else if (n < 0) {
//                     if (errno == EINTR) continue;
//                     if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
//                 }
//
//                 // Client sending a request
//                 else 
//                 std::cout << "Client sent: " << std::string(buf, n) << "\n";
//             }
//         }
//     }
// }

#include "server.hpp"
#include "defaultprotocol.hpp"
#include "tcptransport.hpp"
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

/**/
Server::Server(const std::string &host, const std::string &port, const std::string &protocol) : 
    host(host), port(port), socket(TCPSocket::server_socket(host, port)) {

    
    init_();
    }

/**/
void Server::init_() {
    FD_ZERO(&master_);
    FD_SET(socket.fd(), &master_);
    max_fd_ = socket.fd();
}

/**/
void Server::listening() {
    int listen = ::listen(socket.fd(), SOMAXCONN);
    std::cout << "Listen for connections " << listen << '\n';
    if (listen < 0) {
        throw std::runtime_error("listen failed");
    }
}

/**/
void Server::tick() {
    fd_set fds = master_;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if (select(max_fd_+1, &fds, 0, 0, &timeout) < 0) {
        throw std::runtime_error("select failed");
    }

    for (int fd=0; fd<=max_fd_; ++fd) {
        if (!FD_ISSET(fd, &fds)) continue;

        // Client connecting to the server
        if (fd == socket.fd()) {

            // IDEA IS TO REPLACE THIS BRANCH WITH 
            // accept(fd)

            std::cout << "New client attempting to connect" << "\n";

            // Accept Block
            TCPSocket client_socket = TCPSocket::accept_client(fd);
            std::cout << "After accept call" << "\n";

            if (client_socket.fd() < 0) {
                throw std::runtime_error("Accept Failed");
            }

            int cfd = client_socket.fd();
            // Abstract to set_protocol function
            std::unique_ptr<IProtocol> proto;
            if (protocol_ == "default") 
                proto = std::make_unique<DefaultProtocol>();
            else 
                proto = std::make_unique<DefaultProtocol>();

            // Abstract to set_transport function 
            std::string transport_ = "tcp";
            std::unique_ptr<ITransport> transport;
            if (transport_ == "tcp")
                transport = std::make_unique<TCPTransport>(std::move(client_socket));

            connections.emplace(cfd, Connection{std::move(transport), 
                                                std::move(proto)
                                                });

            // failed to connect, might not need check due to throw.
            if (cfd < 0) continue; 
            FD_SET(cfd, &master_);
            if (cfd > max_fd_) max_fd_ = cfd; 
            std::cout << "New client successfully connected" << "\n";
            // Accept Block end 
        }
        else {
            // IDEA IS TO REPLACE THIS BRANCH WITH 
            // handle_client(fd)

            ssize_t total = 4096;
            uint8_t buf[total];
            // ssize_t n = TCPSocket::recieve(fd, buf);
            ssize_t n = ::recv(fd,buf,sizeof(buf),0);

            // Client disconnecting 
            if (n == 0) {
                FD_CLR(fd, &master_);
                connections.erase(fd);
                if (fd == max_fd_)
                    while (max_fd_ >= 0 && !FD_ISSET(max_fd_, &master_))
                        max_fd_--;
                std::cerr << "Sever: Client has disconnected" << "\n";
            }

            // Error occured
            else if (n < 0) {
                if (errno == EINTR) continue;
                if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
            }

            // Client sending a request
            else if (n > 0) {
                // send the clients message to the inbox
                auto &conn = connections.at(fd);
                conn.protocol->decode(buf, n);
                while (conn.protocol->has_message()) 
                    inbox_.push({fd, conn.protocol->return_message()});
            }
        }
    }
}

/**/
bool Server::has_message() { return !inbox_.empty(); }

/**/
Message Server::next() {
    Message m = std::move(inbox_.front());
    inbox_.pop();
    return m;
}

/**/
void Server::send(int fd, const std::string &buf) {
    auto &conn = connections.at(fd);
    auto bytes = conn.protocol->encode(buf);

    bool status = conn.transport->send_all(bytes.data(), bytes.size());
    if (!status)
        throw std::runtime_error("Server Send Failed");
}

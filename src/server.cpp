#include "server.hpp"
#include "defaultprotocol.hpp"
#include "tcptransport.hpp"
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

/**/
Server::Server(const std::string &host, const std::string &port, 
               const std::string &protocol, const std::string &transport) : 
    host(host), port(port), socket(TCPSocket::server_socket(host, port)), 
    protocol_(protocol), transport_(transport) {
    init_();
    listening_();
    }

/**/
void Server::init_() {
    FD_ZERO(&master_);
    FD_SET(socket.fd(), &master_);
    max_fd_ = socket.fd();
}

/**/
void Server::listening_() {
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
        if (fd == socket.fd()) accept_client_(fd);
        // Client sending message
        else handle_client_(fd);
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

/**/ 
void Server::accept_client_(int fd) {
    TCPSocket client_socket = TCPSocket::accept_client(fd);

    if (client_socket.fd() < 0) { throw std::runtime_error("Accept Failed"); }

    int cfd = client_socket.fd();
    // Dont forget, Sets transport and protocol internally using class functions 
    connections.emplace(cfd, Connection{set_transport_(std::move(client_socket)), 
                                        set_protocol_() });

    if (cfd < 0) return; 
    FD_SET(cfd, &master_);
    if (cfd > max_fd_) max_fd_ = cfd; 
}

/**/ 
void Server::handle_client_(int fd) {
    ssize_t total = 4096;
    uint8_t buf[total];
    ssize_t n = connections.at(fd).transport->recieve(buf);

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
        if (errno == EINTR) return ;
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
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

/**/
std::unique_ptr<IProtocol> Server::set_protocol_() {
    if (protocol_ == "default") 
        return std::make_unique<DefaultProtocol>();
    else 
        return std::make_unique<DefaultProtocol>();
}

/**/
std::unique_ptr<ITransport> Server::set_transport_(TCPSocket &&client_socket) {
    if (transport_ == "tcp")
        return std::make_unique<TCPTransport>(std::move(client_socket));
    else 
        return std::make_unique<TCPTransport>(std::move(client_socket));
}

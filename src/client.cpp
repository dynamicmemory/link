#include "client.hpp"
#include "defaultprotocol.hpp"
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

/**/
Client::Client(const std::string &host, const std::string &port, const std::string &protocol) : 
    host(host), port(port), socket(TCPSocket::client_socket(host, port)) {
    // Add different protocol options as they get build until the weight becomes to heavy
    if (protocol == "default") 
        protocol_ = std::make_unique<DefaultProtocol>();
    else 
        protocol_ = std::make_unique<DefaultProtocol>();

    init_();
    }

/**/
void Client::init_() {
    FD_ZERO(&master_);
    FD_SET(socket.fd(), &master_);
    FD_SET(0, &master_);
}

/**/
void Client::tick() {
    fd_set fds = master_;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if (select(socket.fd()+1, &fds, 0, 0, &timeout) < 0) 
        throw std::runtime_error("client select failed");
    
    if (FD_ISSET(0, &fds)) {
        // User input something, encode with protocol, send to server 
        // Thought a user using this lib, would use .send, so perhaps removal
        // of this clause instead.
    }

    if (FD_ISSET(socket.fd(), &fds)) {
        ssize_t total = 4096;
        uint8_t buf[total];
        ssize_t n = ::recv(socket.fd(),buf,sizeof(buf),0);

        if (n == 0) {
            throw std::runtime_error("Client: Server has disconnected");
            // TODO: Safely handle shutdown
        }

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) 
                return;
            throw std::runtime_error("Client: recv failed");
        }

        protocol_->decode(buf, n);
        while (protocol_->has_message()) 
            inbox_.push({socket.fd(), protocol_->return_message()});
    }
}

/**/
bool Client::has_message() {
    return !inbox_.empty();
}

/**/ 
Message Client::next() {
    Message m = std::move(inbox_.front());
    inbox_.pop();
    return m;
}

/**/ 
void Client::send(const std::string &buf) {
    auto bytes = protocol_->encode(buf);

    bool status = socket.send_all(bytes.data(), bytes.size());
    if (!status)
        throw std::runtime_error("Client send failed");
}

#include "client.hpp"
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

/**/
Client::Client(const std::string &host, const std::string &port) : 
    host(host), port(port), socket(TCPSocket::client_socket(host, port)) {
    init_();
    }

/**/
void Client::init_() {
    FD_ZERO(&master_);
    FD_ISSET(socket.fd(), &master_);
}

/**/
void Client::tick() {
    fd_set fds = master_;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;

    if (select(socket.fd()+1, &fds, 0, 0, &timeout) < 0) 
        throw std::runtime_error("client select failed");
    
    if (FD_ISSET(0, &fds)) {
        // User input something, encode with protocol, send to server 
        // Thought a user using this lib, would use .send, so perhaps removal
        // of this clause instead.
    }

    if (FD_ISSET(socket.fd(), &fds)) {
        // Call protocol to decode message from server, if < 1 server disconnect 
        // if < 0 error, otherwise send on to input for user to use.
        // Random vars for the time being, protocol will return the buf we return
        // to the user
        char buf;
        int n;
        inbox_.push({socket.fd(), std::vector<char>(buf, buf+n)});
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

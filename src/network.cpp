#include "network.hpp"
#include <iostream>

Network::Network() = default;

Server Network::create_server(const std::string &host, 
                              const std::string &port, 
                              const std::string &protocol) {
    Server server(host, port);
    return server;
}

Client Network::create_client(const std::string &host, const std::string &port) {
    Client client(host, port);
    return client;
}

void Network::send() {}
void Network::recieve() {}

void Network::disconnect() {}
void Network::shutdown() {}

#include "network.hpp"
#include <iostream>

Network::Network() = default;

Server Network::create_server(const std::string &host, 
                              const std::string &port, 
                              const std::string &protocol,
                              const std::string &transport) {
    Server server(host, port, protocol, transport);
    return server;
}

Client Network::create_client(const std::string &host, 
                              const std::string &port, 
                              const std::string &protocol,
                              const std::string &transport) {
    Client client(host, port, protocol, transport);
    return client;
}

void Network::send() {}
void Network::recieve() {}

void Network::disconnect() {}
void Network::shutdown() {}

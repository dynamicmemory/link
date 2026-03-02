#include "network.hpp"
#include <iostream>

Network::Network() = default;

Server Network::create_server(const std::string &host, 
                              const std::string &port, 
                              const std::string &protocol) {
    std::string proto = protocol.empty() ? "default" : protocol;
    Server server(host, port, proto);
    return server;
}

Client Network::create_client(const std::string &host, 
                              const std::string &port, 
                              const std::string &protocol) {
    std::string proto = protocol.empty() ? "default" : protocol;
    Client client(host, port, proto);
    return client;
}

void Network::send() {}
void Network::recieve() {}

void Network::disconnect() {}
void Network::shutdown() {}

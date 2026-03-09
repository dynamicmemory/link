#include "network.hpp"
#include <iostream>

Network::Network() = default;

Server Network::create_server(const std::string &port, 
                              const std::string &host, 
                              const std::string &protocol,
                              const std::string &transport,
                              const std::string &multiplexer) {
    Server server(host, port, protocol, transport, multiplexer);
    return server;
}

Client Network::create_client(const std::string &host, 
                              const std::string &port, 
                              const std::string &protocol,
                              const std::string &transport,
                              const std::string &multiplexer) {
    Client client(host, port, protocol, transport, multiplexer);
    return client;
}

void Network::send() {}
void Network::recieve() {}

void Network::disconnect() {}
void Network::shutdown() {}

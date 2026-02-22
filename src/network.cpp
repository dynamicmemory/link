#include "network.hpp"
#include <iostream>

Network::Network() = default;

void Network::create_server(const std::string &host, const std::string &port, const std::string &protocol) {
    std::cout << "Server created" << '\n';
}

void Network::create_client(const std::string &host, const std::string &port) {
    std::cout << "Client created" << '\n';
}

void Network::start_server() {
    std::cout << "Server running" << '\n';
}

void Network::send() {}
void Network::recieve() {}

void Network::disconnect() {}
void Network::shutdown() {}

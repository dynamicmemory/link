/* This class was initially added to contain and manage server and clients, 
 * currently it isn't fulfilling that roll and instead is just a wrapper. */
#include "network.hpp"

Network::Network() = default;

Server Network::create_server(const std::string &host, 
                              const std::string &port, 
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

// Currently not used and possibly never going to be.
void Network::disconnect() {}
void Network::shutdown() {}

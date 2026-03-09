#pragma once 
#include <string>
#include "server.hpp"
#include "client.hpp"

class Network {
private:

public:
    Network();

    Server create_server(const std::string &port, 
                         const std::string &host = "0.0.0.0", 
                         const std::string &protocol = "default",
                         const std::string &transport = "tcp",
                         const std::string &multiplexer = "select");

    Client create_client(const std::string &host, 
                         const std::string &port,
                         const std::string &protocol = "default",
                         const std::string &transport = "tcp",
                         const std::string &multiplexer = "select");

    void start_server(int);

    void send();
    void recieve();

    void disconnect();
    void shutdown();
};



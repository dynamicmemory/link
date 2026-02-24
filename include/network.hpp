#pragma once 
#include <string>
#include "server.hpp"
#include "client.hpp"

class Network {
private:

public:
    Network();

    Server create_server(const std::string &host, const std::string &port, const std::string &protocol);

    Client create_client(const std::string &host, const std::string &port);

    void start_server(int);

    void send();
    void recieve();

    void disconnect();
    void shutdown();
};



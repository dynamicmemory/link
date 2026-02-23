#pragma once 

#include <string>
#include <memory>
#include <vector>

class Network {
private:

public:
    Network();

    int create_server(const std::string &host, const std::string &port, const std::string &protocol);

    void create_client(const std::string &host, const std::string &port);

    void start_server(int);

    void send();
    void recieve();

    void disconnect();
    void shutdown();
};



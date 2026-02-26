#pragma once 
#include <string>
#include "tcpsocket.hpp"

class Client {
private:
    TCPSocket socket; 
    std::string host;
    std::string port;
public:
    Client(const std::string &, const std::string &);
    // void create_client();

    void tick();
    void has_message();
    void next();
    void send();
    void broadcast();
};


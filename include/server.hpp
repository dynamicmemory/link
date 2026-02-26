#pragma once 
#include <string>
#include <unordered_map>
#include "tcpsocket.hpp"

class Server {
private:
    std::string host;
    std::string port;
    int server_socket;
    TCPSocket socket;
    std::unordered_map<int, TCPSocket> connections; // Might need to change to client obj
public:
    Server(const std::string &, const std::string &);
    void listening();
    void start_server();

    void tick();
    void has_message();
    void next();
    void send();
    void broadcast();
};

#pragma once 
#include <string>
#include <queue>
#include "tcpsocket.hpp"
#include "message.hpp"

class Client {
private:
    TCPSocket socket; 
    std::string host;
    std::string port;
    fd_set master_;

    std::queue<Message> inbox_;
public:
    Client(const std::string &, const std::string &);

    void init_();
    void tick();
    bool has_message();
    Message next();
    void send(const std::string &); 
    void recieve();
    void broadcast();
};


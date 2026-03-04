#pragma once 
#include <string>
#include <queue>
#include <memory>
#include "iprotocol.hpp"
#include "tcpsocket.hpp"
#include "message.hpp"
#include "connection.hpp"
#include "itransport.hpp"

class Client {
private:
    TCPSocket socket; 
    std::string host;
    std::string port;
    std::string protocol_;
    std::string transport_;

    fd_set master_;

    Connection connection_; 
    std::queue<Message> inbox_;
public:
    Client(const std::string &, const std::string &, 
           const std::string &, const std::string &);

    void init_();
    void tick();
    bool has_message();
    Message next();
    void send(const std::string &); 

    void broadcast();
    
    std::unique_ptr<IProtocol> set_protocol_();
    std::unique_ptr<ITransport> set_transport_(TCPSocket &&);
};


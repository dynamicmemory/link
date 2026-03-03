#pragma once 
#include <string>
#include <unordered_map>
#include <queue>
#include <memory>
#include "tcpsocket.hpp"
#include "message.hpp"
#include "iprotocol.hpp"
#include "connection.hpp"


class Server {
private:
    std::string host;
    std::string port;
    std::string protocol_;

    int server_socket;
    TCPSocket socket;
    std::unordered_map<int, Connection> connections;

    fd_set master_;
    int max_fd_;

    std::queue<Message> inbox_;
public:
    Server(const std::string &, const std::string &, const std::string &);
    void init_();

    void listening();
    void start_server();

    void tick();
    bool has_message();
    Message next();
    void send(int, const std::string &); 
    void recieve();
    void broadcast();
};

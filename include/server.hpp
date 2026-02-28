#pragma once 
#include <string>
#include <unordered_map>
#include <queue>
#include "tcpsocket.hpp"
#include "message.hpp"

class Server {
private:
    std::string host;
    std::string port;

    int server_socket;
    TCPSocket socket;
    std::unordered_map<int, TCPSocket> connections; // Might need to change to client obj

    fd_set master_;
    int max_fd_;

    std::queue<Message> inbox_;
public:
    Server(const std::string &, const std::string &);
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

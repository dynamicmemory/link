#pragma once 
#include <string>

class Server {
private:
    std::string host;
    std::string port;
    int server_socket;

public:
    Server(const std::string &, const std::string &);
    void listening();
    void start_server();
};

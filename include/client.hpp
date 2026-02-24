#pragma once 

#include <string>

class Client {
private:
    std::string host;
    std::string port;
public:
    Client(const std::string &, const std::string &);
    void create_client();
};


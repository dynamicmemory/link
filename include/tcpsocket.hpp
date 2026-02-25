#pragma once
#include <string>
#include <netdb.h>
#include <unistd.h>

class TCPSocket {
private:
    int fd_;

    static addrinfo *address_(const std::string &, const std::string &, bool);
    static int socket_(addrinfo *);

public: 
    explicit TCPSocket(int);
    ~TCPSocket();

    TCPSocket(const TCPSocket &) = delete; 
    TCPSocket &operator=(const TCPSocket &) = delete;
    TCPSocket(TCPSocket &&);
    TCPSocket &operator=(TCPSocket &&);

    static TCPSocket server_socket(const std::string &, const std::string &);
    static TCPSocket client_socket(const std::string &, const std::string &);

    void listen_socket();
    static TCPSocket accept_client(int);

    ssize_t send_all(); 
    ssize_t recieve_all();

    int fd();
};



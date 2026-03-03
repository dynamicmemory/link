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

    bool send_all(const uint8_t *, size_t); 
    ssize_t recieve(uint8_t *);

    int fd() const;
};



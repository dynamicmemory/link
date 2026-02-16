#include <iostream>
#include <string>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

class TCPSocket {
    int fd{-1};

public: 
    TCPSocket() = default;
    explicit TCPSocket(int fd) : fd(fd) {}    // Keeping for now, may never use.

    ~TCPSocket() { if (fd >= 0) ::close(fd); }

    TCPSocket(const TCPSocket &) = delete;
    TCPSocket &operator=(const TCPSocket &) = delete;

    TCPSocket(TCPSocket &&rhs) { 
        fd = rhs.fd; 
        rhs.fd = -1;
    }

    void server_socket(const std::string &h, const std::string &p) {
        struct addrinfo hints, *bind_addr;
        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        std::cout << "Setting up: " << h << ":" << p << '\n';
        int status = ::getaddrinfo(h.c_str(), p.c_str(), &hints, &bind_addr);
        if (status != 0) {
            throw std::runtime_error(gai_strerror(status));
        }
        std::cout << "getaddrinfo was successful" << '\n';

        fd = ::socket(bind_addr->ai_family, 
                      bind_addr->ai_socktype, 
                      bind_addr->ai_protocol);
        if (fd < 0) {
            throw std::runtime_error("Socket Failed");
        }
        std::cout << "socket was successful, fd: " << fd << '\n';

        bind(bind_addr);

        int listen = ::listen(fd, SOMAXCONN);
        std::cout << "Listen was successful: " << listen << '\n';
    }

    void bind(addrinfo *bind_addr) {
        int bindle = ::bind(fd, bind_addr->ai_addr, bind_addr->ai_addrlen);
        if (bindle != 0) {
            throw std::runtime_error("Bind Failed");
        }
        std::cout << "Bind was successful: " << bindle << '\n';
        freeaddrinfo(bind_addr);
    }

    int client_socket() { return 1;}

    // Abstract out server and client specific constructor ops into these funcs
    void accept() {}
    void connection() {}

    void listen() {}

};


#include <string>

class Client {
    // TCPSocket socket;
public:
    Client(const std::string &host, const std::string &port) {}
        // socket(host, port, true) {}
};


#include <string>
#include <vector>

class Server {
    TCPSocket listener;
    std::vector<TCPSocket> sockets;
    std::string host;
    std::string port;
    std::string protocol;
    // IProtocol protocol;
public:
    Server(const std::string &host, const std::string &port, const std::string &protocol) {
        listener.server_socket(host, port);
    }
};


#include <string>
#include <memory>
#include <vector>

class Network {
    std::vector<std::unique_ptr<Server>> servers;

public:
    Network() {}

    void create_server(const std::string &host, const std::string &port, const std::string &protocol) {
        servers.push_back(std::make_unique<Server>(host, port, protocol));
    }

    void create_client(const std::string &host, const std::string &port);

    void send() {}
    void recieve() {}

    void disconnect() {}
    void shutdown() {}
};


// Main for testing 
int main(int argc, char **argv) {
    Network n;
    std::string host = "127.0.0.1";
    std::string port = "4444";

    n.create_server(host, port, "protocol");

    return 0;
}


class Connection {
public: 
};
 

class Listener {
public: 
};


class Transport {
public: 
};


class IProtocol {
public: 
};


class DefaultProtocol : IProtocol {
public:
};


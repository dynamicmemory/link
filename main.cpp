#include <iostream>
#include <string>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

class TCPSocket {
    int fd;
    addrinfo *addr;

public: 
    TCPSocket() = default;
    ~TCPSocket() { if (fd >= 0) ::close(fd); }

    TCPSocket(const TCPSocket &) = delete;
    TCPSocket &operator=(const TCPSocket &) = delete;

    TCPSocket(TCPSocket &&rhs) { 
        fd = rhs.fd; 
        rhs.fd = -1;
    }

    void open(const std::string &host, const std::string &port) {
        struct addrinfo hints; 
        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        std::cout << "Setting up: " << host << ":" << port << '\n';

        int status = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
        if (status != 0) { throw std::runtime_error(gai_strerror(status)); }
        std::cout << "getaddrinfo was successful" << '\n';

        int fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (fd < 0) { throw std::runtime_error("Socket Failed"); }
        std::cout << "socket was successful, fd: " << fd << '\n';
    }

    /*Binds a socket to the address contained in the bind_addr param*/
    void bind() {
        int bindle = ::bind(fd, addr->ai_addr, addr->ai_addrlen);
        if (bindle != 0) {
            throw std::runtime_error("Bind Failed");
        }
        std::cout << "Bind was successful: " << bindle << '\n';
        freeaddrinfo(addr);
    }

    /**/
    void listen() {
        int listen = ::listen(fd, SOMAXCONN);
        std::cout << "Listen was successful: " << listen << '\n';
    }
    /**/
    void connect(addrinfo *connect_addr) { /* Write connect logic*/ }
    /**/
    void accept() { /* Write accept logic*/ }
};


#include <string>
#include <vector>

class Server {
    TCPSocket socket;
    std::vector<TCPSocket> sockets;
    std::string host;
    std::string port;
    std::string protocol;
public:
    Server(const std::string &h, const std::string &p, const std::string &protocol) {
        socket.open(h, p);
        socket.bind();
        socket.listen();
    }
};








#include <string>

class Client {
    TCPSocket socket;
public:
    Client(const std::string &host, const std::string &port) {}

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


#include <iostream>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include <cstring>

class TCPSocket {
    int fd = -1;

    static addrinfo *address(const std::string &host, const std::string &port, bool passive) {
        struct addrinfo hints, *addr; 
        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_UNSPEC;
        if (passive) hints.ai_flags = AI_PASSIVE;
        std::cout << "Setting up: " << host << ":" << port << '\n';

        int status = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
        if (status != 0) { throw std::runtime_error(gai_strerror(status)); }
        std::cout << "getaddrinfo was successful" << '\n';

        return addr;
    }

    static int make_socket(addrinfo *addr) {
        int fd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (fd < 0) { throw std::runtime_error("Socket Failed"); }
        std::cout << "socket was successful, fd: " << fd << '\n';

        return fd;
    }

public: 
    explicit TCPSocket(int fd) : fd(fd) {}
    ~TCPSocket() { if (fd >= 0) ::close(fd); }

    TCPSocket(const TCPSocket &) = delete;
    TCPSocket &operator=(const TCPSocket &) = delete;

    TCPSocket(TCPSocket &&rhs) { 
        fd = rhs.fd; 
        rhs.fd = -1;
    }

    static TCPSocket server_socket(const std::string &host, const std::string &port) {
        struct addrinfo *addr = address(host, port, true); 
        int fd = make_socket(addr);

        int b = ::bind(fd, addr->ai_addr, addr->ai_addrlen);
        if (b != 0) {
            ::close(fd);
            freeaddrinfo(addr);
            throw std::runtime_error("Bind Failed");
        }
        std::cout << "Bind was successful: " << b << '\n';
        freeaddrinfo(addr);

        return TCPSocket(fd);
    }

    static TCPSocket client_socket(const std::string &host, const std::string &port) {
        struct addrinfo *addr = address(host, port, false); 
        int fd = make_socket(addr);

        int c = ::connect(fd, addr->ai_addr, addr->ai_addrlen);
        if (c != 0) {
            ::close(fd);
            freeaddrinfo(addr);
            throw std::runtime_error("Connect Failed");

        }
        std::cout << "connection was successful: " << c << '\n';
        freeaddrinfo(addr);

        return TCPSocket(fd);
    }

    /**/
    void listen() {
        int listen = ::listen(fd, SOMAXCONN);
        std::cout << "Listen for connections " << listen << '\n';
    }

    /**/
    TCPSocket accept_client() {
        std::cout << "Start of accept" << "\n";
        struct sockaddr client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_socket = ::accept(fd, (struct sockaddr *)&client_addr, 
                                   &addr_len);
        std::cout << "After accept call" << "\n";

        if (client_socket < 0) {
            throw std::runtime_error("Accept Failed");
        }

        std::cout << "Client accepted" << "\n";
        return TCPSocket(client_socket);
        
    }

    ssize_t send_all() { /*Write send_all logic*/ return 0; }
    ssize_t recieve_all() { /*Write send_all logic*/ return 0; }
};


#include <string>
#include <vector>
// #include "TCPSocket.cpp"

class Server {
    TCPSocket socket;
    std::vector<TCPSocket> clients;
    std::string host;
    std::string port;
    std::string protocol;
public:
    Server(const std::string &h, const std::string &p, const std::string &protocol) : 
        socket(TCPSocket::server_socket(h, p)) {
        socket.listen();

        socket.accept_client();
        }
};


#include <string>

class Client {
    TCPSocket socket;
public:
    Client(const std::string &host, const std::string &port) : 
        socket(TCPSocket::client_socket(host, port)) {}
};

/* Interface for a protocol for a server */
class IProtocol {
public: 
};


/* Simple byte based protocol with a prelength message assigned, etc. Default 
 * for the library, more protocols will be build once up and running.*/
class DefaultProtocol : IProtocol {
public:
};


/* Owns the servers and the clients, top level object that exposes basic apis 
 * for a user to use to create and manager network */
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

    void create_client(const std::string &host, const std::string &port) {
        Client(host, port);
    }

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
    // n.create_client(host, port);

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


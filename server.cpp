/////////////////////////////// TCPSocket ////////////////////////////////////

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

    TCPSocket &operator=(TCPSocket &&rhs) {
        if (this == &rhs) return *this;
        if (fd > 0) {
            ::close(fd);
            fd = rhs.fd;
            rhs.fd = -1;
        }
        return *this;
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

    int get_fd() { return fd; }
};

//////////////////////////////////// CONNECTION ///////////////////////////////


class Connection {};

/////////////////////////////////// SERVER ///////////////////////////////////

#include <string>
#include <vector>
// #include "TCPSocket.cpp"

class Server {
    TCPSocket socket;
    std::vector<TCPSocket> connections;
    std::string host;
    std::string port;
    std::string protocol;
public:
    Server(const std::string &h, const std::string &p, const std::string &protocol) : 
        socket(TCPSocket::server_socket(h, p)) {
        }

    void start_server() {
        socket.listen();
        socket.accept_client();

        fd_set master;
        int max_fd = socket.get_fd();
        
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        while (1) {
            fd_set fds = master;
            if (select(max_fd+1, &fds, 0, 0, &timeout) < 0) {
                throw std::runtime_error("select failed");
            }

            for (int fd=0; fd<=max_fd; ++fd) {
                if (!FD_ISSET(fd, &fds)) continue;

                // Client connecting to the server
                if (fd == socket.get_fd()) {
                    TCPSocket new_client = socket.accept_client();
                    // failed to connect, might not need check due to throw.
                    if (new_client.get_fd() < 0) continue; 
                    connections.push_back(std::move(new_client));
                    FD_SET(new_client.get_fd(), &master);
                    if (new_client.get_fd() > max_fd) max_fd = new_client.get_fd();

                }
                else {
                    ssize_t total = 1024;
                    char buf[total];
                    ssize_t n = ::recv(fd,buf,total,0);

                    // Client disconnecting 
                    if (n == 0) {
                        // Consider using find instead of the loop
                        for (auto it=connections.begin(); it != connections.end(); ++it)
                            if (it->get_fd() == fd) it = connections.erase(it);

                        FD_CLR(fd, &master);
                        if (fd == max_fd)
                            while (max_fd >= 0 && !FD_ISSET(max_fd, &fds))
                                max_fd--;
                        std::cerr << "Client has disconnected" << "\n";
                    }
                    // Client sending a request
                    continue;
                }
            }
        }
    }

    void connect() {}
    // void add_client(TCPSocket client_socket) {
    //     connections.push_back(client_socket.get_fd())
    // }
};

//////////////////////////////// CLINET //////////////////////////////////////

#include <string>

class Client {
    TCPSocket socket;
public:
    Client(const std::string &host, const std::string &port) : 
        socket(TCPSocket::client_socket(host, port)) {}
};

///////////////////////////// NETWORK API ///////////////////////////////////

/* Owns the servers and the clients, top level object that exposes basic apis 
 * for a user to use to create and manager network */
#include <string>
#include <memory>
#include <vector>

class Network {
    std::vector<std::unique_ptr<Server>> servers;
    std::vector<std::unique_ptr<Client>> clients;

public:
    Network() {}

    void create_server(const std::string &host, const std::string &port, const std::string &protocol) {
        servers.push_back(std::make_unique<Server>(host, port, protocol));
    }

    void create_client(const std::string &host, const std::string &port) {
        clients.push_back(std::make_unique<Client>(Client(host, port)));
    }

    void start_server() {
        // Hard coded only first server for now.
        servers[0]->start_server();
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
    n.start_server();
    // n.create_client(host, port);

    return 0;
}

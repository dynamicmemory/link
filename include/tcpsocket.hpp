#include <string>
#include <netdb.h>
#include <unistd.h>

class TCPSocket {
private:
    int fd;

    static addrinfo *address(const std::string &, const std::string &, bool);
    static int make_socket(addrinfo *);

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
    TCPSocket accept_client();

    ssize_t send_all(); 
    ssize_t recieve_all();

    int get_fd();
};



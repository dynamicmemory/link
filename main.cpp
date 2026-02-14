#include <string>
#include <netdb.h>
#include <sys/socket.h>

class TCPSocket {
public: 
    TCPSocket(const std::string &h, const std::string &p) : host(h), port(p) {}

    void accept() {}
    void connection() {}
    void bind() {}

private:
    const std::string host;
    const std::string port;
};


class Connection {
public: 

private:

};
 

class Listener {
public: 

private:

};


class Transport {
public: 

private:

};


class IProtocol {
public: 

private:

};


class DefaultProtocol : IProtocol {
public:

private:

};


class Server {
public:

private:

};


#include <string>

class Client {
public:
    Client(const std::string &host, const std::string &port) : sock(host, port) {}

private:
    TCPSocket sock;

};


#include <string>
using const_string = std::string &;

class Network {
public:
    Network() {}

    void create_server(const_string host, const_string port, const_string protocol);
    void create_client(const_string host, const_string protocol);

    void send() {}
    void recieve() {}

    void disconnect() {}
    void shutdown() {}

private:

};


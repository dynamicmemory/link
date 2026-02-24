#include <string>
#include "network.hpp"
#include "server.hpp"

int main(void) {

    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "None";
    Network network;

    Server server = network.create_server(host, port, protocol);
    server.listening();
    server.start_server();

    return 0;
}

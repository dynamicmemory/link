#include <iostream>
#include <string>
#include "network.hpp"
#include "server.hpp"

int main(void) {

    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "";
    Network network;

    Server server = network.create_server(host, port, protocol);
    server.listening();
    
    while (1) {
        server.tick();

        if (server.has_message()) {
            auto message = server.next();
            std::cout << message.payload << "\n";

            server.send(message.fd, "I have received your message, how exciting");
        }
    }

    return 0;
}

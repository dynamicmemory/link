#include <iostream>
#include <string>
#include "network.hpp"
#include "client.hpp"

int main(void) {
    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "newline";
    std::string transport = "tcp";
    Network n;
    Client client = n.create_client(host, port, protocol, transport);

    while (1) {
        if (!client.is_ready()) continue;
        client.tick();
        
        client.send("Hello I am the client, I am sending you this message... how exciting");
        
        if(client.has_message()) {
            auto message = client.next();
            std::cout << message.payload << "\n";

        }
    }

    return 0;
}


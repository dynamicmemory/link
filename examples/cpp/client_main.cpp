#include <iostream>
#include <string>
#include "network.hpp"
#include "client.hpp"

int main(void) {
    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "newline";
    std::string transport = "tls";
    Network n;

    Client client = n.create_client(host, port, protocol, transport);

    int p = 0;
    while (1) {
        client.tick();
        if (client.is_ready() && p == 0) {
            client.send("Hello I am the client, I am sending you this message... how exciting");
            p++;
        }
        
        if(client.has_message()) {
            auto message = client.next();
            std::cout << message.payload << "\n";

        }
    }

    return 0;
}


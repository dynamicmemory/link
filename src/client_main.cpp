#include <iostream>
#include <string>
#include "network.hpp"
#include "client.hpp"

int main(void) {
    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "None";
    Network n;

    Client client = n.create_client(host, port);
    
    client.send("Ping");
    while (1) {
        client.tick();
        
        if(client.has_message()) {
            auto message = client.next();
            std::cout << message.payload << "\n";

        }
    }

    return 0;
}


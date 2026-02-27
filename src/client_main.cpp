#include <string>
#include "network.hpp"
#include "client.hpp"

int main(void) {
    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "None";
    Network n;

    Client client = n.create_client(host, port);
    
    while (1)
        client.tick();

    return 0;
}


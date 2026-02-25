#include <string>
#include "network.hpp"
#include "client.hpp"

int main(void) {

    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "None";
    Network n;

    // Probably should auto create a client rather then double call?
    Client client = n.create_client(host, port);
    // client.create_client();
    
    while (1) ;

    return 0;
}


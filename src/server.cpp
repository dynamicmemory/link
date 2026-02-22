#include <string>
#include "network.hpp"

int main(void) {

    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "None";
    Network n;

    n.create_server(host, port, protocol);
    

    return 0;
}

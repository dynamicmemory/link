#include <string>
#include "network.hpp"

int main(void) {

    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "None";
    Network n;

    int listening = n.create_server(host, port, protocol);
    n.start_server(listening);


    return 0;
}

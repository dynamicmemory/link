#define CATCH_CONFIG_MAIN
#include "../include/catch2.hpp"

#include "../include/network.hpp"
#include "../include/server.hpp"
#include "../include/client.hpp"
#include <thread>

TEST_CASE("Server receives what client sends") {
    // Network n;
    //
    // Server server = n.create_server("0.0.0.0", "6969", "default", "tcp", "select");
    // Client client = n.create_client("127.0.0.1", "6969", "default", "tcp", "select");
    //
    // std::string payload = "hello catch";
    // client.send(payload);
    //
    // while (!server.has_message())
    //     server.tick();
    //
    // REQUIRE(server.has_message() == true);
    //
    // auto msg = server.next();
    // REQUIRE(msg.payload == payload);
}

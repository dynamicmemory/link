#define CATCH_CONFIG_MAIN
#include "../include/catch2.hpp"

#include "../include/network.hpp"
#include "../include/server.hpp"
#include "../include/client.hpp"
#include <thread>

TEST_CASE("Server receives what client sends") {
    Network n;

    Server server = n.create_server("0.0.0.0", "6969", "default", "tcp", "select");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Client client = n.create_client("127.0.0.1", "6969", "default", "tcp", "select");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::string payload = "hello catch";

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    client.send(payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    while (!server.has_message())
        server.tick();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    REQUIRE(server.has_message() == true);

    auto msg = server.next();
    REQUIRE(msg.payload == payload);
}

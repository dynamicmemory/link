#include "../include/catch2.hpp"
#include "../include/prefixedlengthprotocol.hpp"

#include <cstring>
#include <arpa/inet.h>

TEST_CASE("PrefixedLengthProtocolEncodes message with length prefix") {
    PrefixedLengthProtocol p;
    // Test encode & encode_length by definition
    auto data = p.encode("hello");
    REQUIRE(data.size() == 9);

    // Test encode_length
    uint32_t elen;
    std::memcpy(&elen, data.data(), 4);
    REQUIRE(ntohl(elen) == 5);
}

TEST_CASE("PrefixedLengthProtocol: Decodes framed messages") {
    PrefixedLengthProtocol p;

    // Test has message 
    REQUIRE_FALSE(p.has_message());

    // Test decode & decode_length 
    auto data = p.encode("hello");
    p.decode(data.data(), data.size());
    REQUIRE(p.has_message());

    // Test return_message 
    auto msg = p.return_message();
    REQUIRE(msg == "hello");
}

TEST_CASE("PrefixedLengthProtocol: Handles partial frames") {
    PrefixedLengthProtocol p;

    auto data = p.encode("hello");
    p.decode(data.data(), 3);
    REQUIRE_FALSE(p.has_message());

    p.decode(data.data()+3, data.size()-3);
    REQUIRE(p.has_message());
    REQUIRE(p.return_message() == "hello");
}

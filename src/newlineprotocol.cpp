#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include <algorithm>
#include "newlineprotocol.hpp"

/**/
NewLineProtocol::NewLineProtocol(uint32_t maxsize) : MAX_FRAME(maxsize) {} 

/*
 * Appends newline delimiter to message
 */
std::vector<uint8_t> NewLineProtocol::encode(const std::string &message) {
    if (message.size() > MAX_FRAME)
        throw std::runtime_error("Message exceeds maximum allowed size");

    // std::cout << "Encoding: " << message << std::endl;

    // Add the '\n' delim for decode to find
    std::vector<uint8_t> data(message.begin(), message.end());
    data.push_back('\n');
    return data;
} 

/*
 * Incremental decode using delimiter search:
 *  - append incoming bytes
 *  - extract messages when '\n' is found
 */
void NewLineProtocol::decode(const uint8_t *data, size_t len) {
    buff_.insert(buff_.end(), data, data+len);


    while (true) {
        auto it = std::find(buff_.begin(), buff_.end(), '\n');
        if (it == buff_.end())
            break; // recv didn't get the whole message

        std::string m(buff_.begin(), it);
        while (!m.empty() && (m.back() == '\n' || m.back() == '\r')) m.pop_back();
        // std::cout << "Decoded: " << m << std::endl;
        messages_.push(std::move(m));

        // Clear storage +1 for the newline
        buff_.erase(buff_.begin(), it + 1);
    }
}

/* Returns true if a fully decoded message is available, false otherwise. */
bool NewLineProtocol::has_message() const {
    return !messages_.empty();
}

/* Returns next fully decoded message. */
std::string NewLineProtocol::return_message() {
    if (messages_.empty())
        throw std::runtime_error("No message available");

    auto message = std::move(messages_.front());
    messages_.pop();
    return message;
}

// TODO: Review Protocol interface, perhaps remove these two as mandatory and 
//       implement in protocols where needed.
void NewLineProtocol::encode_length(uint8_t *data, uint32_t len) {
}
uint32_t NewLineProtocol::decode_length(uint8_t *data) {
    uint32_t place_holder;
    return place_holder;
}


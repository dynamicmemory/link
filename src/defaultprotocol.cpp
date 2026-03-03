#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include "defaultprotocol.hpp"

/**/
DefaultProtocol::DefaultProtocol(uint32_t maxsize) : MAX_FRAME(maxsize) {} 

/**/
void DefaultProtocol::encode_length(uint8_t *data, uint32_t len) {
    uint32_t prefix = htonl(len);
    std::memcpy(data, &prefix, sizeof(prefix));
}

/**/
uint32_t DefaultProtocol::decode_length(uint8_t *data) {
    uint32_t prefix;
    std::memcpy(&prefix, data, sizeof(prefix));
    return ntohl(prefix);
}

/**/
std::vector<uint8_t> DefaultProtocol::encode(const std::string &message) {
    if (message.size() > MAX_FRAME)
        throw std::runtime_error("Message exceeds maximum allowed size");

    std::vector<uint8_t> data(4 + message.size());
    encode_length(data.data(), message.size());
    std::copy(message.begin(), message.end(), data.begin() + 4);
    return data;
} 

/**/
void DefaultProtocol::decode(const uint8_t *data, size_t len) {
    buff_.insert(buff_.end(), data, data+len);

    while (buff_.size() >= 4) {
        uint32_t m_len = decode_length(buff_.data());

        if (m_len > MAX_FRAME) 
            throw std::runtime_error("Message exceeds maximum allowed size");

        if (buff_.size() < 4 + m_len)
            break; // recv didn't get the whole message

        std::string m(buff_.begin() + 4, buff_.begin() + 4 + m_len);
        messages_.push(std::move(m));

        buff_.erase(buff_.begin(), buff_.begin() + 4 + m_len);
    }
}

/**/
bool DefaultProtocol::has_message() const {
    return !messages_.empty();
}

/**/
std::string DefaultProtocol::return_message() {
    if (messages_.empty())
        throw std::runtime_error("No message available");

    auto message = std::move(messages_.front());
    messages_.pop();
    return message;
}


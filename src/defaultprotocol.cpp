#include <stdexcept>
#include "defaultprotocol.hpp"

/**/
DefaultProtocol::DefaultProtocol(uint32_t maxsize) : MAX_FRAME(maxsize) {} 

/**/
void DefaultProtocol::encode_prefix(uint8_t *data, uint32_t len) {
    data[0] = (len >> 24) & 0xFF;
    data[1] = (len >> 16) & 0xFF;
    data[2] = (len >> 8) & 0xFF;
    data[3] = len & 0xFF;
}

/**/
std::vector<uint8_t> DefaultProtocol::encode(const std::string &message) {
    if (message.size() > MAX_FRAME)
        throw std::runtime_error("Message exceeds maximum allowed size");

    std::vector<uint8_t> data(4 + message.size());
    encode_prefix(data.data(), message.size());
    std::copy(message.begin(), message.end(), data.begin() + 4);
    return data;
} 

/**/
uint32_t DefaultProtocol::decode_prefix(uint8_t *data) {
    return (uint32_t(data[0]) << 24) | (uint32_t(data[1]) << 16) |
           (uint32_t(data[2]) << 8)  |  uint32_t(data[3]);
}

/**/
void DefaultProtocol::decode(const uint8_t *data, size_t len) {
    buff_.insert(buff_.end(), data, data+len);

    while (buff_.size() >= 4) {
        uint32_t m_len = decode_prefix(buff_.data());

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


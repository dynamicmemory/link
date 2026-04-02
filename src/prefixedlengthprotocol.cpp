/* PrefixedLengthProtocol implements a simple length-prefixed message protocol.
 * Each message is transmitted using the following frame format:
 *
 *     [4-byte length prefix][message payload]
 *
 * The length prefix is a 32-bit unsigned integer encoded in network byte order 
 * (big-endian) and represents the number of bytes contained in the payload.
 *
 * Example frame:
 *
 *     00 00 00 05   48 65 6C 6C 6F
 *     | length=5 |   "Hello"
 *
 * Encode outgoing messages into framed byte sequences. Decode incoming byte 
 * streams into discrete messages. */
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include "prefixedlengthprotocol.hpp"

/**/
PrefixedLengthProtocol::PrefixedLengthProtocol(uint32_t maxsize) : MAX_FRAME(maxsize) {} 

/* Encodes a payload length into network byte order. The length prefix is 
 * written into the provided buffer using big-endian encoding so that the 
 * protocol remains platform independent across architectures.
 * @param data Pointer to the buffer where the encoded length will be written.
 * @param len  Payload size in bytes. */
void PrefixedLengthProtocol::encode_length(uint8_t *data, uint32_t len) {
    uint32_t prefix = htonl(len);
    std::memcpy(data, &prefix, sizeof(prefix));
}

/* Decodes a length prefix from network byte order. Reads the first four bytes 
 * of the provided buffer and converts them from network byte order to host 
 * byte order.
 * @param data Pointer to the buffer containing the length prefix.
 * @return Decoded payload length. */
uint32_t PrefixedLengthProtocol::decode_length(uint8_t *data) {
    uint32_t prefix;
    std::memcpy(&prefix, data, sizeof(prefix));
    return ntohl(prefix);
}

/* Encodes a message into a length-prefixed frame.
 * @param message Application message to encode.
 * @return Byte vector containing the encoded frame. */
std::vector<uint8_t> PrefixedLengthProtocol::encode(const std::string &message) {
    if (message.size() > MAX_FRAME)
        throw std::runtime_error("Message exceeds maximum allowed size");

    std::vector<uint8_t> data(4 + message.size());
    encode_length(data.data(), message.size());
    std::copy(message.begin(), message.end(), data.begin() + 4);
    return data;
} 

/* Processes incoming data from the network. This function accumulates received 
 * bytes into an internal buffer until a complete frame is available.
 *
 * @param data Pointer to newly received bytes.
 * @param len  Number of bytes received from the transport. */
void PrefixedLengthProtocol::decode(const uint8_t *data, size_t len) {
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

/* Returns true if at least one decoded message is ready, false otherwise. */
bool PrefixedLengthProtocol::has_message() const {
    return !messages_.empty();
}

/* @return Next decoded message. */
std::string PrefixedLengthProtocol::return_message() {
    if (messages_.empty())
        throw std::runtime_error("No message available");

    auto message = std::move(messages_.front());
    messages_.pop();
    return message;
}

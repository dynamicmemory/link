#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include "defaultprotocol.hpp"

/**/
DefaultProtocol::DefaultProtocol(uint32_t maxsize) : MAX_FRAME(maxsize) {} 

/* Encodes a payload length into network byte order.
 * The length prefix is written into the provided buffer
 * using big-endian encoding so that the protocol remains
 * platform independent across architectures.
 *
 * @param data Pointer to the buffer where the encoded length will be written.
 * @param len  Payload size in bytes. */
void DefaultProtocol::encode_length(uint8_t *data, uint32_t len) {
    uint32_t prefix = htonl(len);
    std::memcpy(data, &prefix, sizeof(prefix));
}

/* Decodes a length prefix from network byte order.
 * Reads the first four bytes of the provided buffer and
 * converts them from network byte order to host byte order.
 * @param data Pointer to the buffer containing the length prefix.
 *
 * @return Decoded payload length. */
uint32_t DefaultProtocol::decode_length(uint8_t *data) {
    uint32_t prefix;
    std::memcpy(&prefix, data, sizeof(prefix));
    return ntohl(prefix);
}

/* Encodes a message into a length-prefixed frame.
 * The resulting byte sequence is structured as:
 *     [4 byte length][payload bytes]
 * @param message Application message to encode.
 * @return Byte vector containing the encoded frame. */
std::vector<uint8_t> DefaultProtocol::encode(const std::string &message) {
    if (message.size() > MAX_FRAME)
        throw std::runtime_error("Message exceeds maximum allowed size");

    std::vector<uint8_t> data(4 + message.size());
    encode_length(data.data(), message.size());
    std::copy(message.begin(), message.end(), data.begin() + 4);
    return data;
} 

/* Processes incoming data from the network.
 * Because TCP delivers a continuous byte stream rather than
 * discrete messages, this function accumulates received bytes
 * into an internal buffer until a complete frame is available.
 *
 * @param data Pointer to newly received bytes.
 * @param len  Number of bytes received from the transport. */
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

/* Indicates whether a decoded message is available.
 * @return true if at least one fully decoded message is ready to be consumed. */
bool DefaultProtocol::has_message() const {
    return !messages_.empty();
}

/* @return Next decoded message. */
std::string DefaultProtocol::return_message() {
    if (messages_.empty())
        throw std::runtime_error("No message available");

    auto message = std::move(messages_.front());
    messages_.pop();
    return message;
}

/* DefaultProtocol / prefixlength Protocol
 *
 * Implements a simple length-prefixed message protocol.
 * Each message is transmitted using the following frame format:
 *
 *     [4-byte length prefix][message payload]
 *
 * The length prefix is a 32-bit unsigned integer encoded in
 * network byte order (big-endian) and represents the number
 * of bytes contained in the payload.
 *
 * Example frame:
 *
 *     00 00 00 05   48 65 6C 6C 6F
 *     | length=5 |   "Hello"
 *
 * Responsibilities:
 * - Encode outgoing messages into framed byte sequences.
 * - Decode incoming byte streams into discrete messages.
 * - Handle partial network reads by buffering incomplete frames.
 *
 * This protocol operates on a byte stream and therefore must
 * internally accumulate data until a full message frame has
 * been received.
 *
 * Safety:
 * - Frames larger than MAX_FRAME are rejected to prevent
 *   memory exhaustion or malformed protocol input.
 */

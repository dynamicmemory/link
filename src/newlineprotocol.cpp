#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include <algorithm>
#include "newlineprotocol.hpp"

/**/
NewLineProtocol::NewLineProtocol(uint32_t maxsize) : MAX_FRAME(maxsize) {} 

/* Encodes a message for transport by adding the '\n' char to the end.
 * @param message Application message to encode.
 * @return Byte vector containing the encoded frame. */
std::vector<uint8_t> NewLineProtocol::encode(const std::string &message) {
    if (message.size() > MAX_FRAME)
        throw std::runtime_error("Message exceeds maximum allowed size");

    // Add the '\n' delim for decode to find
    std::vector<uint8_t> data(message.begin(), message.end());
    data.push_back('\n');
    return data;
} 

/* Processes incoming data from the network.
 * Because TCP delivers a continuous byte stream rather than
 * discrete messages, this function accumulates received bytes
 * into an internal buffer until a complete frame is available
 * using a newline char as the control.
 *
 * @param data Pointer to newly received bytes.
 * @param len  Number of bytes received from the transport. */
void NewLineProtocol::decode(const uint8_t *data, size_t len) {
    buff_.insert(buff_.end(), data, data+len);

    while (true) {
        auto it = std::find(buff_.begin(), buff_.end(), '\n');
        if (it == buff_.end())
            break; // recv didn't get the whole message

        std::string m(buff_.begin(), buff_.end());
        messages_.push(std::move(m));

        // Clear storage +1 for the newline
        buff_.erase(buff_.begin(), it + 1);
    }
}

/* Indicates whether a decoded message is available.
 * @return true if at least one fully decoded message is ready to be consumed. */
bool NewLineProtocol::has_message() const {
    return !messages_.empty();
}

/* @return Next decoded message. */
std::string NewLineProtocol::return_message() {
    if (messages_.empty())
        throw std::runtime_error("No message available");

    auto message = std::move(messages_.front());
    messages_.pop();
    return message;
}

/* Not used in newline
 * @param data Pointer to the buffer where the encoded length will be written.
 * @param len  Payload size in bytes. */
void NewLineProtocol::encode_length(uint8_t *data, uint32_t len) {
}

/* Not used in newline 
 * @param data Pointer to the buffer containing the length prefix.
 * @return Decoded payload length. */
uint32_t NewLineProtocol::decode_length(uint8_t *data) {
    uint32_t place_holder;
    return place_holder;
}
/* NewLineProtocol
 *
 * Implements a simple newline delimited message protocol.
 * Each message is transmitted using the following frame format:
 *
 *     [message payload][\n]
 *
 * Responsibilities:
 * - Encode outgoing messages with a \n char at the end.
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

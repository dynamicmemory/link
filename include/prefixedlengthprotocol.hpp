#pragma once 
#include <vector>
#include <string>
#include <queue>
#include <cstdint>
#include "iprotocol.hpp"

/**
 * PrefixedLengthProtocol
 *
 * A message framing protocol using a fixed-size length prefix.
 *
 * Frame format:
 *   [4-byte length][payload bytes]
 *
 * - Length is a 32-bit unsigned integer in network byte order (big-endian)
 * - Payload is raw message data of specified length
 *
 * Example:
 *   00 00 00 05  48 65 6C 6C 6F
 *   [ length=5 ] "Hello"
 *
 * Responsibilities:
 *  - encode messages into framed byte sequences
 *  - decode byte streams into complete messages
 *
 * Buffering behavior:
 *  - incoming bytes are accumulated in an internal buffer
 *  - incomplete frames are retained until sufficient data arrives
 *  - multiple messages may be decoded from a single input chunk
 *
 * Constraints:
 *  - messages larger than MAX_FRAME are rejected
 */
class PrefixedLengthProtocol : public IProtocol {
private:
    std::queue<std::string> messages_;
    std::vector<uint8_t> buff_;
    uint32_t MAX_FRAME; 
public:
    /**
     * @param MAX_FRAME Maximum allowed message size in bytes
     */
    PrefixedLengthProtocol(uint32_t MAX_FRAME = 1 << 20);
    ~PrefixedLengthProtocol() override = default;

    /**
     * Encodes a payload length into 4-byte big-endian format.
     */
    void encode_length(uint8_t *, uint32_t) override;

    /**
     * Encodes a message into a length-prefixed frame.
     *
     * @throws std::runtime_error if message exceeds MAX_FRAME
     */
    std::vector<uint8_t> encode(const std::string &) override;

    /**
     * Decodes a 4-byte big-endian length prefix.
     */
    std::uint32_t decode_length(uint8_t *) override;

    /**
     * Consumes raw bytes and attempts to extract complete messages.
     *
     * - may produce zero or more messages
     * - retains incomplete frames internally
     *
     * @throws std::runtime_error if frame exceeds MAX_FRAME
     */
    void decode(const uint8_t *, size_t) override;

    /**
     * Returns true if at least one decoded message is available.
     */
    bool has_message() const override;
    /**
     * Returns and removes the next decoded message.
     *
     * @throws std::runtime_error if no message is available
     */
    std::string return_message() override;
};


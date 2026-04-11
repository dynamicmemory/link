#pragma once 
#include <vector>
#include <string>
#include <queue>
#include <cstdint>
#include "iprotocol.hpp"

/**
 * NewLineProtocol
 *
 * A delimiter-based message framing protocol using '\n' as a terminator.
 *
 * Frame format:
 *   [payload bytes]['\n']
 *
 * Example:
 *   "hello\nworld\n"
 *   → "hello", "world"
 *
 * Responsibilities:
 *  - encode messages by appending '\n'
 *  - decode byte streams into messages using newline delimiter
 *
 * Buffering behavior:
 *  - incoming data is accumulated in an internal buffer
 *  - messages are extracted when a '\n' delimiter is found
 *  - partial messages remain buffered until completed
 *
 * Notes:
 *  - '\r\n' is normalized by stripping trailing '\r'
 *  - messages exceeding MAX_FRAME are rejected
 */
class NewLineProtocol : public IProtocol {
private:
    std::queue<std::string> messages_;
    std::vector<uint8_t> buff_;
    uint32_t MAX_FRAME; 
public:
    /**
     * @param MAX_FRAME Maximum allowed message size in bytes
     */
    NewLineProtocol(uint32_t MAX_FRAME = 1 << 20);
    ~NewLineProtocol() override = default;

    /**
     * Encodes a message by appending '\n'
     *
     * @throws std::runtime_error if message exceeds MAX_FRAME
     */
    std::vector<uint8_t> encode(const std::string &) override;

    /**
     * Consumes raw bytes and extracts newline-delimited messages.
     *
     * - may produce zero or more messages
     * - retains incomplete messages internally
     */
    void decode(const uint8_t *, size_t) override;

    /**
     * Returns true if at least one message is available.
     */
    bool has_message() const override;

    /**
     * Returns and removes the next decoded message.
     *
     * @throws std::runtime_error if no message is available
     */
    std::string return_message() override;

    // Unused in delimeter-based proto, knwown flaw in IProtocol, will be 
    // removed from core interface in future updates.
    std::uint32_t decode_length(uint8_t *) override;
    // Unused in delimeter-based proto, knwown flaw in IProtocol, will be 
    // removed from core interface in future updates.
    void encode_length(uint8_t *, uint32_t) override;
};


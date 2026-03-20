#pragma once 
#include <vector>
#include <string>
#include <queue>
#include <cstdint>
#include "iprotocol.hpp"

class PrefixedLengthProtocol : public IProtocol {
private:
    std::queue<std::string> messages_;
    std::vector<uint8_t> buff_;
    uint32_t MAX_FRAME; 
public:
    PrefixedLengthProtocol(uint32_t MAX_FRAME = 1 << 20);
    ~PrefixedLengthProtocol() override = default;

    void encode_length(uint8_t *, uint32_t) override;
    std::vector<uint8_t> encode(const std::string &) override;

    std::uint32_t decode_length(uint8_t *) override;
    void decode(const uint8_t *, size_t) override;

    bool has_message() const override;
    std::string return_message() override;
};


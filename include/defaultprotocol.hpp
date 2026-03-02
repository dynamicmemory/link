#pragma once 
#include <vector>
#include <string>
#include <queue>
#include <cstdint>
#include "iprotocol.hpp"

class DefaultProtocol : public IProtocol {
private:
    std::queue<std::string> messages_;
    std::vector<uint8_t> buff_;
    uint32_t MAX_FRAME; 
public:
    DefaultProtocol(uint32_t MAX_FRAME = 1 << 20);
    ~DefaultProtocol() override = default;

    void encode_prefix(uint8_t *, uint32_t) override;
    std::vector<uint8_t> encode(const std::string &) override;

    std::uint32_t decode_prefix(uint8_t *) override;
    void decode(const uint8_t *, size_t) override;

    bool has_message() const override;
    std::string return_message() override;
};


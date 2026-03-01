#pragma once 
#include <vector>
#include <string>
#include <queue>
#include <cstdint>
#include "iprotocol.hpp"

class DefaultProtocol : public IProtocol {
private:
    std::queue<std::string> messages_;
    std::vector<uint8_t> recv_buffer_;
    uint32_t MAX_FRAME; 

public:
    DefaultProtocol(uint32_t MAX_FRAME = 1 << 20);
    ~DefaultProtocol() override = default;

    bool has_message() const override;
    std::string return_message() override;

    void decode(const uint8_t *, size_t) override;
    void encode(const std::string &) override;
};

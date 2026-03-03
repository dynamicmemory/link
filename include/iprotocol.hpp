#pragma once 
#include <vector>
#include <string>
#include <cstdint>

class IProtocol {
public:
    virtual ~IProtocol() = default;

    virtual void encode_length(uint8_t *, uint32_t) = 0;
    virtual std::vector<uint8_t> encode(const std::string &) = 0;

    virtual uint32_t decode_length(uint8_t *) = 0;
    virtual void decode(const uint8_t *, size_t) = 0;

    virtual bool has_message() const = 0;
    virtual std::string return_message() = 0;
};

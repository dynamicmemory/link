#pragma once 
#include <vector>
#include <string>
#include <cstdint>

class IProtocol {
public:
    virtual ~IProtocol() = default;

    virtual bool has_message() const = 0;
    virtual std::string return_message() = 0;

    virtual void decode(const uint8_t *, size_t) = 0;
    virtual void encode(const std::string &) = 0;
};

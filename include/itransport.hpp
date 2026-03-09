#pragma once 

#include <unistd.h>
#include <cstdint>

class ITransport {
public:
    virtual ~ITransport() = default;

    virtual int fd() const = 0;
    virtual ssize_t recieve(uint8_t *, size_t) = 0;
    virtual bool send_all(const uint8_t *, size_t) = 0;
};

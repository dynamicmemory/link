#pragma once 

#include "itransport.hpp"
#include "tcpsocket.hpp"

class TCPTransport : public ITransport {
private:
    TCPSocket socket_;
public:
    explicit TCPTransport(TCPSocket);

    int fd() const override;
    ssize_t recieve(uint8_t *, size_t) override;
    bool send_all(const uint8_t *, size_t) override;
    bool is_ready() override;
};

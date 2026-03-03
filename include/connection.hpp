#pragma once 

#include <memory>
#include "iprotocol.hpp"
#include "itransport.hpp"

struct Connection {
    std::unique_ptr<ITransport> transport;
    std::unique_ptr<IProtocol> protocol;

    int fd() const { return transport->fd(); }
};

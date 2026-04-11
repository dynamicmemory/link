#pragma once 

#include <memory>
#include "iprotocol.hpp"
#include "itransport.hpp"

/**
 * Connection
 *
 * Represents a single network connection in the system.
 *
 * Each Connection encapsulates:
 * - An ITransport instance (e.g., TCPTransport)
 * - An IProtocol instance (e.g., DefaultProtocol)
 *
 * Responsibilities:
 * - Provide a unified interface to send and receive messages.
 * - Maintain per-connection state, including message buffers.
 * - Expose the underlying file descriptor for multiplexing.
 *
 * - transport and protocol must always be valid.
 * - fd() always returns the descriptor of the underlying transport.
 * - Messages received via the protocol are stored internally until
 *   consumed.
 *
 * Usage:
 * - The Server and Client classes maintain a map of fd → Connection.
 * - Incoming and outgoing data is routed through Connection for
 *   encoding/decoding and transport I/O.
 */
struct Connection {
    std::unique_ptr<ITransport> transport;
    std::unique_ptr<IProtocol> protocol;

    int fd() const { return transport->fd(); }
};



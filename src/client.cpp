#include "client.hpp"
#include "defaultprotocol.hpp"
#include "tcptransport.hpp"
#include "selectmultiplexer.hpp"
#include <netdb.h>
#include <unistd.h>

/**/
Client::Client(const std::string &host, 
               const std::string &port, 
               const std::string &protocol, 
               const std::string &transport,
               const std::string &multiplexer) : 
    host(host), port(port), socket(TCPSocket::client_socket(host, port)),
    protocol_(protocol), transport_(transport), multistrategy_(multiplexer) {
    set_multiplexer_();
    init_();
    }

/**/
void Client::init_() {
    connection_ = Connection{set_transport_(std::move(socket)), set_protocol_() };
    multiplexer_->add_fd(connection_.fd());
    connected_ = true;
}

/* Processes I/O events for the client.
 *
 * - Waits on the multiplexer to see if data is ready.
 * - Reads incoming messages and decodes them into the inbox.
 * - Handles server disconnection or read errors.
 *
 * Must be called repeatedly in the client’s main loop to maintain
 * responsiveness.
 */
void Client::tick() {
    if (!connected_) return;

    multiplexer_->wait(0);

    if (multiplexer_->ready(connection_.fd())) {
        size_t size = 4096;
        uint8_t buf[size];
        ssize_t n = connection_.transport->recieve(buf, size);

        if (n == 0) {
            connected_ = false;
            inbox_.push({connection_.fd(), "Server disconnected"});
            return;
        }

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) 
                return;
            throw std::runtime_error("Client: recv failed");
        }

        connection_.protocol->decode(buf, n);
        while (connection_.protocol->has_message()) 
            inbox_.push({connection_.fd(), connection_.protocol->return_message()});
    }
}

/* Returns whether the server has any fully decoded messages available.
 * @return true if inbox contains messages, false otherwise. */
bool Client::has_message() {
    return !inbox_.empty();
}

/* Retrieves the next message from the inbox.
 * @return Next available message. */
Message Client::next() {
    Message m = std::move(inbox_.front());
    inbox_.pop();
    return m;
}

/* Returns the current clients connections status to the server*/ 
bool Client::is_connected() {
    return connected_;
}

/* Sends a message to the connected server.
 *
 * - Encodes the message via the protocol layer.
 * - Transmits the entire encoded frame via the transport.
 *
 * @param buf Message string to send.
 */
void Client::send(const std::string &buf) {
    auto bytes = connection_.protocol->encode(buf);

    bool status = connection_.transport->send_all(bytes.data(), bytes.size());
    if (!status)
        throw std::runtime_error("Client: send failed");
}

/* Configures the protocol object for new connections */
std::unique_ptr<IProtocol> Client::set_protocol_() {
    if (protocol_ == "default") 
        return std::make_unique<DefaultProtocol>();
    else 
        return std::make_unique<DefaultProtocol>();
}

/* Configures the transport object for a new connection*/
std::unique_ptr<ITransport> Client::set_transport_(TCPSocket &&socket) {
    if (transport_ == "tcp")
        return std::make_unique<TCPTransport>(std::move(socket));
    else 
        return std::make_unique<TCPTransport>(std::move(socket));
}

/* Configures the multiplexer for the server.*/
void Client::set_multiplexer_() {
    if (multistrategy_ == "select")
        multiplexer_ = std::make_unique<SelectMultiplexer>();
    else 
        multiplexer_ = std::make_unique<SelectMultiplexer>();
}





/**
 * Client
 *
 * Represents a single TCP client connection to a remote server.
 *
 * Responsibilities:
 * - Establish a connection to a specified host and port.
 * - Manage a single Connection instance (transport + protocol).
 * - Use a multiplexer to check readiness for I/O without blocking.
 * - Encode outgoing messages and decode incoming messages.
 * - Provide a message inbox for application consumption.
 *
 * - connection_ always contains a valid transport and protocol.
 * - The multiplexer accurately tracks the connection file descriptor.
 * - Messages in the inbox are fully decoded and ready for retrieval.
 *
 * Limitations:
 * - Currently uses default TCP transport, length-prefixed protocol,
 *   and select-based multiplexing.
 */

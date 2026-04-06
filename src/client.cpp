/* Represents a TCP client connection with transport, protocol and multiplexer.
 * options. Uses transport layer to internally encode and decode messages.
 * Main use is to provide the library users with a basic set of APIs to use 
 * in their project.
 */

#include "client.hpp"
#include "prefixedlengthprotocol.hpp"
#include "newlineprotocol.hpp"
#include "tcptransport.hpp"
#include "tlstransport.hpp"
#include "selectmultiplexer.hpp"
#include "netevent.hpp"

#include <netdb.h>
#include <unistd.h>

/* Client constructor: sets up host, port, protocol, transport and multiplexer*/
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

/* Initialize connection and register with multiplexer.*/
void Client::init_() {
    connection_ = Connection{set_transport_(std::move(socket)), set_protocol_() };
    multiplexer_->add_fd(connection_.fd());
    connected_ = true;
}

/* Processes I/O events for the client.
 * - Waits on the multiplexer to see if data is ready.
 * - Reads incoming messages and decodes them into the inbox.
 * - Handles server disconnection or read errors. 
 * @param timeout - Three base options < 0 will create blocking multiplexer 
                    behavior.
                  - 0 will create non-blocking polling like behavior.
                  - > 0 will timeout the client until the timeout period is over.
*/
void Client::tick(int timeout) {
    if (!connected_) return;

    multiplexer_->wait(timeout);

    if (multiplexer_->ready(connection_.fd())) {
        if (!connection_.transport->is_ready())
            return;

        size_t size = 4096;
        uint8_t buf[size];
        ssize_t n = connection_.transport->recieve(buf, size);

        if (n == 0) {
            connected_ = false;
            inbox_.push({connection_.fd(), NetEvent::SERVER_DISCONNECT, ""});
            return;
        }

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) 
                return;
            throw std::runtime_error("Client: recv failed");
        }

        connection_.protocol->decode(buf, n);
        while (connection_.protocol->has_message()) 
            inbox_.push({connection_.fd(), 
                         NetEvent::DATA,
                         connection_.protocol->return_message()});
    }
}

/* Returns true if the client has fully decoded message and false if not.*/
bool Client::has_message() {
    return !inbox_.empty();
}

/* Retrieves the next message from the inbox. */
Message Client::next() {
    Message m = std::move(inbox_.front());
    inbox_.pop();
    return m;
}

/* Returns the current clients connections status to the server */ 
bool Client::is_connected() {
    return connected_;
}

/* Sends a message to the connected server.
 * - Encodes the message via the protocol layer.
 * - Transmits the entire encoded frame via the transport. */
void Client::send(const std::string &buf) {
    auto bytes = connection_.protocol->encode(buf);

    bool status = connection_.transport->send_all(bytes.data(), bytes.size());
    if (!status)
        throw std::runtime_error("Client: send failed");
}

/* Configures the protocol object for new connections */
std::unique_ptr<IProtocol> Client::set_protocol_() {
    if (protocol_ == "default") 
        return std::make_unique<PrefixedLengthProtocol>();
    else if (protocol_ == "newline")
        return std::make_unique<NewLineProtocol>();
    else 
        return std::make_unique<PrefixedLengthProtocol>();
}

/* Configures the transport object for a new connection*/
std::unique_ptr<ITransport> Client::set_transport_(TCPSocket &&socket) {
    if (transport_ == "tcp")
        return std::make_unique<TCPTransport>(std::move(socket));
    else if (transport_ == "tls")
        return std::make_unique<TLSTransport>(std::move(socket), false);
    else 
        return std::make_unique<TCPTransport>(std::move(socket));
}

/* Configures the multiplexer for the client.*/
void Client::set_multiplexer_() {
    if (multistrategy_ == "select")
        multiplexer_ = std::make_unique<SelectMultiplexer>();
    else 
        multiplexer_ = std::make_unique<SelectMultiplexer>();
}

/* Returns true if socket is ready for communicaiton, false otherwise*/
bool Client::is_ready() {
    return connection_.transport->is_ready();
}


/* */
void Client::verify_certificate(std::string &cert) { 
    TLSTransport *tls = dynamic_cast<TLSTransport*>(connection_.transport.get()); 
    if (!tls) throw std::runtime_error("Transport is not tls"); 
    tls->verify_server_cert(cert); 
}

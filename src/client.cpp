#include "client.hpp"
#include "prefixedlengthprotocol.hpp"
#include "newlineprotocol.hpp"
#include "tcptransport.hpp"
#include "tlstransport.hpp"
#include "selectmultiplexer.hpp"
#include "netevent.hpp"

#include <netdb.h>
#include <unistd.h>

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

/*
 * Initializes the connection pipeline:
 *  - wraps the raw socket in a transport implementation
 *  - attaches the configured protocol encoder/decoder
 *  - registers the socket with the multiplexer
 *
 * After this call, the client is considered connected at transport level.
 */
void Client::init_() {
    connection_ = Connection{
        set_transport_(std::move(socket)), 
        set_protocol_() 
    };

    multiplexer_->add_fd(connection_.fd());
    connected_ = true;
}

/*
 * Event loop step for the client.
 *
 * This function:
 *  - waits for socket readiness via the multiplexer
 *  - reads raw bytes from the transport layer
 *  - feeds bytes into the protocol decoder
 *  - extracts complete messages into the inbox queue
 *
 * Connection failure handling:
 *  - n == 0  → peer closed connection
 *  - n < 0   → transient error (EAGAIN/EWOULDBLOCK) or fatal error
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

        // The server has disonnected
        if (n == 0) {
            connected_ = false;
            inbox_.push({connection_.fd(), NetEvent::SERVER_DISCONNECT, ""});
            return;
        }

        // Data not finished reading or error
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) 
                return;
            throw std::runtime_error("Client: recv failed");
        }

        // Decode the message and send it to the inbox ready for the user.
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

/*
 * Encodes and sends a message through:
 *  protocol → transport → socket
 *
 * The call is synchronous and will attempt to send the full buffer.
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

/*
 * TLS-only utility:
 * Compares provided certificate against the server certificate.
 *
 * Throws if:
 *  - transport is not TLS
 *  - certificate validation fails
 */
void Client::verify_certificate(std::string &cert) { 
    TLSTransport *tls = dynamic_cast<TLSTransport*>(connection_.transport.get()); 
    if (!tls) throw std::runtime_error("Transport is not tls"); 
    tls->verify_server_cert(cert); 
}

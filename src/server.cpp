/* Server represents a TCP server that can accept multiple client connections
 * and handle message-based communication using pluggable transport, protocol 
 * and multiplexing layers. Also provides a set of APIs for a user to use when 
 * constructing a server.
 *
 * - Call tick() in an event loop to process I/O and accept new clients.
 * - Use has_message() and next() to retrieve client messages.
 * - Send messages using send(fd, message).
 */

#include "server.hpp"
#include "prefixedlengthprotocol.hpp"
#include "newlineprotocol.hpp"
#include "tcptransport.hpp"
#include "tlstransport.hpp"
#include "selectmultiplexer.hpp"
#include "netevent.hpp"

#include <netdb.h>
#include <unistd.h>

/* Server constructor: sets up host, port, protocol, transport and multiplexer*/
Server::Server(const std::string &host, const std::string &port, 
               const std::string &protocol, const std::string &transport,
               const std::string &multiplexer) : 
    port(port), host(host), socket(TCPSocket::server_socket(host, port)), 
    protocol_(protocol), transport_(transport), multistrategy_(multiplexer) {
    init_();
    socket.listen_socket();
    }

/* Initialises multiplexer and registers the servers listening socket*/
void Server::init_() {
    set_multiplexer_();
    multiplexer_->add_fd(socket.fd());
}

/* Processes I/O events for the server. 
 * - Waits on the multiplexer for socket readiness events.
 * - Accepts new client connections if the listening socket is ready.
 * - Reads incoming messages from ready clients and stores them in the inbox.
 * - Handles internal clean ups for socket disconnects. */
void Server::tick(int timeout) {
    multiplexer_->wait(timeout);
    if (multiplexer_->ready(socket.fd()))
        accept_client_(socket.fd());

    for (auto &[fd, conn] : connections)
        if (multiplexer_->ready(fd)) {
            // Check for handshake based transport implementations 
            if (!conn.transport->is_ready()) continue;
            handle_client_(fd);
        }

    for (int fd : disconnected_fds) {
        multiplexer_->remove_fd(fd);
        connections.erase(fd);
        // std::cerr << "Sever: Client has disconnected" << "\n";
    }
    disconnected_fds.clear();
}

/* Accepts a new client on the listening socket, wraps the socket in a connections 
 * object along with its selected transport and protocol objects*/
void Server::accept_client_(int fd) {
    TCPSocket client_socket = TCPSocket::accept_client(fd);

    if (client_socket.fd() < 0) { throw std::runtime_error("Accept Failed"); }

    int cfd = client_socket.fd();
    // Dont forget, Sets transport and protocol internally using class functions 
    connections.emplace(cfd, Connection{set_transport_(std::move(client_socket)), 
                                        set_protocol_() });

    if (cfd < 0) return; 
    multiplexer_->add_fd(cfd);
}

/* Handles incoming messages for a specific client, reads data from the 
 * transport, decodes messages via the protocol, and enqueues them in the inbox. 
 * Handles client disconnection. */
void Server::handle_client_(int fd) {
    ssize_t size = 4096;
    uint8_t buf[size];
    ssize_t n = connections.at(fd).transport->recieve(buf, size);

    // Client disconnecting 
    if (n == 0) {
        disconnected_fds.push_back(fd);
        inbox_.push({fd, NetEvent::CLIENT_DISCONNECT, ""});
        return;
    }
    // Error occured
    else if (n < 0) {
        if (errno == EINTR) return ;
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
    }
    // Client sending a request
    else if (n > 0) {
        // send the clients message to the inbox
        auto &conn = connections.at(fd);
        conn.protocol->decode(buf, n);
        while (conn.protocol->has_message()) 
            inbox_.push({fd, NetEvent::DATA, conn.protocol->return_message()});
    }
}

/* Returns true if the server has a fully decoded message, false otherwise */  
bool Server::has_message() { return !inbox_.empty(); }

/* Returns the next available message from the servers inbox. */
Message Server::next() {
    Message m = std::move(inbox_.front());
    inbox_.pop();
    return m;
}

/* Sends a message to a specific client. Encodes the message via the client's 
 * protocol, then writes it to the transport.
 * @param fd File descriptor of the target client.
 * @param buf Message string to send. */
void Server::send(int fd, const std::string &buf) {
    auto &conn = connections.at(fd);
    auto bytes = conn.protocol->encode(buf);

    bool status = conn.transport->send_all(bytes.data(), bytes.size());
    if (!status)
        throw std::runtime_error("Server: Send Failed");
}

void Server::kick(int fd) {
    multiplexer_->remove_fd((fd));
}

/* Configures the protocol object for new connections */
std::unique_ptr<IProtocol> Server::set_protocol_() {
    if (protocol_ == "default") 
        return std::make_unique<PrefixedLengthProtocol>();
    else if (protocol_ == "newline")
        return std::make_unique<NewLineProtocol>();
    else 
        return std::make_unique<PrefixedLengthProtocol>();
}

/* Configures the transport object for a new connection*/
std::unique_ptr<ITransport> Server::set_transport_(TCPSocket &&client_socket) {
    if (transport_ == "tcp")
        return std::make_unique<TCPTransport>(std::move(client_socket));
    else if (transport_ == "tls")
        return std::make_unique<TLSTransport>(std::move(client_socket), true);
    else 
        return std::make_unique<TCPTransport>(std::move(client_socket));
}

/* Configures the multiplexer for the server.*/
void Server::set_multiplexer_() {
    if (multistrategy_ == "select")
        multiplexer_ = std::make_unique<SelectMultiplexer>();
    else 
        multiplexer_ = std::make_unique<SelectMultiplexer>();
}

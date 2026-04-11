#include "server.hpp"
#include "prefixedlengthprotocol.hpp"
#include "newlineprotocol.hpp"
#include "tcptransport.hpp"
#include "tlstransport.hpp"
#include "selectmultiplexer.hpp"
#include "netevent.hpp"

#include <netdb.h>
#include <unistd.h>

Server::Server(const std::string &host, const std::string &port, 
               const std::string &protocol, const std::string &transport,
               const std::string &multiplexer) : 
    port(port), host(host), socket(TCPSocket::server_socket(host, port)), 
    protocol_(protocol), transport_(transport), multistrategy_(multiplexer) {
    init_();
    socket.listen_socket();
    }

/*
 * Initializes multiplexer and registers the listening socket.
 * After this, the server is ready to accept incoming connections.
 */
void Server::init_() {
    set_multiplexer_();
    multiplexer_->add_fd(socket.fd());
}

/*
 * Event loop step for the server.
 *
 * Execution order:
 *  1. Wait for I/O readiness
 *  2. Accept new connections (if listening socket is ready)
 *  3. Process readable client sockets
 *  4. Clean up disconnected clients
 *
 * Important:
 *  - Client removal is deferred to avoid iterator invalidation
 *    during iteration over `connections`. 
 */
void Server::tick(int timeout) {
    multiplexer_->wait(timeout);

    // New client 
    if (multiplexer_->ready(socket.fd()))
        accept_client_(socket.fd());

    // Connected clients messages
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

/*
 * Accepts a new client connection and registers it:
 *  - wraps raw socket in transport layer
 *  - attaches protocol decoder
 *  - stores in connection map
 *  - registers fd with multiplexer
 */
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

/*
 * Handles incoming data for a single client.
 *
 * recv() semantics:
 *  - n > 0  → bytes received
 *  - n == 0 → peer closed connection
 *  - n < 0  → error or non-blocking retry
 */
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

/*
 * Sends a message to a specific client:
 *  protocol → transport → socket
 */
void Server::send(int fd, const std::string &buf) {
    auto &conn = connections.at(fd);
    auto bytes = conn.protocol->encode(buf);

    bool status = conn.transport->send_all(bytes.data(), bytes.size());
    if (!status)
        throw std::runtime_error("Server: Send Failed");
}

/*
 * Removes a client from multiplexer.
 * Note: does NOT erase from connections immediately.
 */
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

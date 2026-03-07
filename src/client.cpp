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
}

/**/
void Client::tick() {
    multiplexer_->wait(0);

    if (multiplexer_->ready(connection_.fd())) {
        uint8_t buf[4096];
        ssize_t n = connection_.transport->recieve(buf);

        if (n == 0) {
            throw std::runtime_error("Client: Server has disconnected");
            // TODO: Safely handle shutdown
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

/**/
bool Client::has_message() {
    return !inbox_.empty();
}

/**/ 
Message Client::next() {
    Message m = std::move(inbox_.front());
    inbox_.pop();
    return m;
}

/**/ 
void Client::send(const std::string &buf) {
    auto bytes = connection_.protocol->encode(buf);

    bool status = connection_.transport->send_all(bytes.data(), bytes.size());
    if (!status)
        throw std::runtime_error("Client: send failed");
}

/**/
std::unique_ptr<IProtocol> Client::set_protocol_() {
    if (protocol_ == "default") 
        return std::make_unique<DefaultProtocol>();
    else 
        return std::make_unique<DefaultProtocol>();
}

/**/
std::unique_ptr<ITransport> Client::set_transport_(TCPSocket &&socket) {
    if (transport_ == "tcp")
        return std::make_unique<TCPTransport>(std::move(socket));
    else 
        return std::make_unique<TCPTransport>(std::move(socket));
}

/**/
void Client::set_multiplexer_() {
    if (multistrategy_ == "select")
        multiplexer_ = std::make_unique<SelectMultiplexer>();
    else 
        multiplexer_ = std::make_unique<SelectMultiplexer>();
}

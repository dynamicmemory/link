#include "network_c_api.h"
#include "../include/server.hpp"
#include "../include/client.hpp"
#include "../include/message.hpp"

#include <string>

struct NetworkServer { Server *impl; };
struct NetworkClient { Client *impl; };

//Server 
NetworkServer *network_server_create(const char *host, const char *port,
        const char *protocol, const char *transport, const char *multiplexer) {
    NetworkServer *s = new NetworkServer;
    s->impl = new Server(port, host, protocol, transport, multiplexer);
    return s;
}

void network_server_tick(NetworkServer *server, int timeout) {
    server->impl->tick(timeout);
}

int network_server_has_message(NetworkServer *server) {
    return server->impl->has_message();
}

const char *network_server_next(NetworkServer *server, int *fd) {
    static std::string msg;
    auto m = server->impl->next();
    *fd = m.fd;
    msg = m.payload;
    return msg.c_str();
}

void network_server_send(NetworkServer *server, int fd, const char *msg) {
    server->impl->send(fd, msg); 
}

void network_server_destroy(NetworkServer *server) {
    delete server->impl;
    delete server;
}


//Client 
NetworkClient *network_client_create(const char *host, const char *port,
        const char *protocol, const char *transport, const char *multiplexer) {
    NetworkClient *s = new NetworkClient;
    s->impl = new Client(host, port, protocol, transport, multiplexer);
    return s;
}

void network_client_tick(NetworkClient *client, int timeout) {
    client->impl->tick(timeout);
}

int network_client_has_message(NetworkClient *client) {
    return client->impl->has_message();
}

const char *network_client_next(NetworkClient *client) {
    static std::string msg;
    auto m = client->impl->next();
    msg = m.payload;
    return msg.c_str();
}

void network_client_send(NetworkClient *client, const char *msg) {
    client->impl->send(msg); 
}

void network_client_destroy(NetworkClient *client) {
    delete client->impl;
    delete client;
}

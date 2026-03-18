#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NetworkServer NetworkServer;
typedef struct NetworkClient NetworkClient;

// SERVER 
NetworkServer *server_create(
    const char *host, const char *port, const char *protocol, 
    const char *transport, const char *multiplexer
    );

void server_tick(NetworkServer *, int );
int server_has_message(NetworkServer *);
const char *server_next(NetworkServer *, int *);
void server_send(NetworkServer *, int , const char *);
void server_destroy(NetworkServer *);

// CLIENT
NetworkClient *client_create(
    const char *host, const char *port, const char *protocol, 
    const char *transport, const char *multiplexer
    );

void client_tick(NetworkClient *, int );
int client_has_message(NetworkClient *);
const char *client_next(NetworkClient *);
void client_send(NetworkClient *, const char *);
int client_is_ready(NetworkClient *);
void client_destroy(NetworkClient *);

#ifdef __cplusplus 
}
#endif 

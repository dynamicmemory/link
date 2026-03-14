#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NetworkServer NetworkServer;
typedef struct NetworkClient NetworkClient;

// SERVER 
NetworkServer *network_server_create(
    const char *, const char *, const char *, const char *, const char *
    );

void network_server_tick(NetworkServer *, int );
int network_server_has_message(NetworkServer *);
const char *network_server_next(NetworkServer *, int *);
void network_server_send(NetworkServer *, int , const char *);
void network_server_destroy(NetworkServer *);

// CLIENT
NetworkClient *network_client_create(
    const char *, const char *, const char *, const char *, const char * 
    );

void network_client_tick(NetworkClient *, int );
int network_client_has_message(NetworkClient *);
const char *network_client_next(NetworkClient *);
void network_client_send(NetworkClient *, const char *);
void network_client_destroy(NetworkClient *);

#ifdef __cplusplus 
}
#endif 
